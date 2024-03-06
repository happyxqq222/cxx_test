//
// Created by Administrator on 2024-02-25.
//

#include "pcb.h"
#include "CommonConnectionPool.h"
#include <exception>
#include <thread>
#include <chrono>

//线程安全的懒汉单列函数接口
ConnectionPool *ConnectionPool::getConnectionPool() {
    static ConnectionPool pool;  //有编译器生成的加锁和解锁 懒汉模式
    return &pool;
}

ConnectionPool::ConnectionPool() {

      //load config file
    loadConfigFile();
    //create connection pool
    for (int i = 0; i < initSize; i++) {
        Connection *p = new Connection;
        if(p->connect(ip, port, username, password, dbname)){
            _connectionQueue.push(p);
            _connectionCnt++;
        }else{
            LOG("failure to connection db ip:"+ip+",port:" + std::to_string(port)+",username:"+username+",password:"+password+",dbname:"+dbname);
            throw "create connection error";
        }
    }

    //start a new thread to be a connection producer
    std::thread produceThreadTask( std::bind(&ConnectionPool::produceConnectionTask,this));
    produceThreadTask.detach();
    //producerThread = std::move(produceThreadTask);

    //start a new thread for scan more than maxidleTime
    std::thread scannerThreadTask(std::bind(&ConnectionPool::scanConectionTask,this));
    scannerThreadTask.detach();
    //scannerThread = std::move(scannerThreadTask);
}

std::shared_ptr<Connection> ConnectionPool::getConnection() {

    std::unique_lock<std::mutex> lock(_queueMutex);

    while(_connectionQueue.empty()){
        if(std::cv_status::timeout == cv.wait_for(lock,std::chrono::milliseconds(connectionTimeout))){
            if(_connectionQueue.empty()){
                LOG("failure to get idle connection!  ")
                return nullptr;
            }
        }
    }

    Connection* conn = _connectionQueue.front();
    std::shared_ptr<Connection> sp(conn,std::bind(&ConnectionPool::recyleConnection,this,conn));
    _connectionQueue.pop();
    if(_connectionQueue.empty()){
        cv.notify_all(); //
    }
    return sp;
}

bool ConnectionPool::loadConfigFile() {
    std::ifstream fileStream("mysql.ini",std::ios::in);
    FileStreamHelper helper(fileStream); //close ifstream helper t

    std::string line;
    while(std::getline(fileStream, line)){
        int idx = line.find('=',0);
        if(idx == -1){  //无效的配置
            continue;
        }
        std::string key = line.substr(0,idx);
        std::string value = line.substr(idx+1);
        if(key == "ip"){
            ip = std::move(value);
        }else if(key == "port"){
            port = std::stoi(value);
        }else if(key == "username"){
            username = std::move(value);
        }else if(key == "password"){
            password = std::move(value);
        }else if(key == "initSize"){
            initSize = std::stoi(value);
        }else if(key == "maxSize"){
            maxSize = std::stoi(value);
        }else if(key == "maxIdleTime"){
            maxIdleTime = std::stoi(value);
        }else if(key == "connectionTimeOut"){
            connectionTimeout = std::stoi(value);
        }else if(key == "dbname"){
            dbname = std::move(value);
        }
    }
    return true;
}

void ConnectionPool::produceConnectionTask() {
    for(;;){
        std::unique_lock<std::mutex> lock(_queueMutex);

        while(!_connectionQueue.empty()){
            cv.wait(lock);
        }

        if(_connectionCnt < maxSize){
            Connection *p = new Connection;
            p->connect(ip,port,username,password,dbname);
            p->refreshAliveTime();
            _connectionQueue.push(p);
            _connectionCnt++;
        }
        cv.notify_all();
    }
}

void ConnectionPool::recyleConnection(Connection* conn) {
    std::unique_lock<std::mutex> lock(_queueMutex);
    if(conn){
        conn->refreshAliveTime();
        this->_connectionQueue.push(conn);
    }
}

void ConnectionPool::scanConectionTask() {
    for(;;){
        std::this_thread::sleep_for(std::chrono::seconds(maxIdleTime));
        //扫描整个队列，释放多余的连接
        std::unique_lock<std::mutex> lock(_queueMutex);
        while(_connectionCnt > initSize){
            Connection * p = _connectionQueue.front();
            if(p->getAliveTime() >=  (maxIdleTime *1000)){
                _connectionQueue.pop();
                _connectionCnt--;
                delete p;
            }else{
                break;
            }
        }
    }
}

ConnectionPool::~ConnectionPool() {
    while(!_connectionQueue.empty()){
        Connection* conn  = _connectionQueue.front();
        _connectionQueue.pop();
        if(conn != nullptr){
            delete conn;
        }
    }
}

