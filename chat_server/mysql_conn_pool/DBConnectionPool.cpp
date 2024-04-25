//
// Created by Administrator on 2024-04-04.
//

#include "DBConnectionPool.h"

#include <memory>
#include <chrono>
#include <functional>

using namespace std;

DBConnectionPool::DBConnectionPool(std::string_view poolName, std::string_view dbServerIp,
                                   uint16_t dbServerPort, std::string_view userName,
                                   std::string_view password, std::string_view dbName, int maxConnCnt)
        : _poolName(poolName), _dbServerIp(dbServerIp), _dbServerPort(dbServerPort),
          username(userName), password(password), dbName(dbName),
          _maxConnectionCount(maxConnCnt), _initSize(5), _maxIdleTime(5),_connectionCount(0) {

    for (int i = 0; i < _initSize; i++) {
        DBConnection dbConn{this};
        if (!dbConn.connect()) {
            throw 1;
        }
        dbConn.refreshAliveTime();
        DBConnection *pDbConn = new DBConnection(std::move(dbConn));
        _freeList.emplace_back(pDbConn);
        _connectionCount++;
    }

    thread t(std::bind(&DBConnectionPool::scannerConnectionTask, this));
    scannerTask = std::move(t);
}

DBConnectionPool::~DBConnectionPool() {
    lock_guard<mutex> lock(_mutex);
    _abortRequest = true;
    _condVar.notify_all();

    for (const auto &it: _freeList) {
        delete it;
    }

    if (scannerTask.joinable()) {
        scannerTask.join();
    }
}


std::shared_ptr<DBConnection> DBConnectionPool::getDbConn() {
    unique_lock<mutex> lock(_mutex);
    if (_abortRequest) {
        return nullptr;
    }
    if (_freeList.empty()) {
        //第一步先检测当前连接数量是否达到最大的连接数量
        if (_connectionCount >= _maxConnectionCount) {
            _condVar.wait(lock,[this](){
                return !_freeList.empty() || _abortRequest;
            });
            if(_abortRequest){
                return nullptr;
            }
        } else {  //还没到最大连接 创建连接
            DBConnection dbConn{this};
            if (!dbConn.connect()) {
                printf("Init DBConnecton failed\n\n");
                return nullptr;
            } else {
                DBConnection* pDbConn = new DBConnection(std::move(dbConn));
                _freeList.push_back(pDbConn);
                _connectionCount++;
            }
        }
    }
    auto dbConn = _freeList.front();
    if(!dbConn){
        cout << "error" << endl;
    }
    _freeList.pop_front();

    cout << this_thread::get_id() << " wakeup connection" << endl;
    return shared_ptr<DBConnection>(dbConn, std::bind(&DBConnectionPool::releaseConn, this, placeholders::_1));;
}

void DBConnectionPool::releaseConn(DBConnection *dbConn) {
    if (!dbConn) {
        return;
    }
    unique_lock<mutex> lock(_mutex);
    dbConn->refreshAliveTime();
    _freeList.push_back(dbConn);
    _condVar.notify_all();
}

void DBConnectionPool::scannerConnectionTask() {
    while (!_abortRequest) {
        if (_abortRequest) {
            break;
        }
        //扫描整个队列，释放多余连接
        unique_lock<mutex> lock(_mutex);
        while (_freeList.size() > _initSize) {
            DBConnection *p = _freeList.front();
            if (p->getAliveTime() >= _maxIdleTime) {
                _freeList.pop_front();
                _connectionCount--;
                delete p;
            } else {
                break;
            }
        }
    }
}
