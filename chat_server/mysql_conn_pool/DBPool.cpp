//
// Created by Administrator on 2024-04-04.
//

#include "DBPool.h"

#include <memory>
#include <chrono>
#include <functional>

using namespace std;

DBPool::DBPool(std::string_view poolName, std::string_view dbServerIp,
               uint16_t dbServerPort, std::string_view userName,
               std::string_view password, std::string_view dbName, int maxConnCnt)
        : mPoolName(poolName), mDbServerIp(dbServerIp), mDbServerPort(dbServerPort),
          mUsername(userName), mPassword(password), mDbName(dbName),
          mDbMaxConnCnt(maxConnCnt), mInitSize(5), mMaxIdleTime(5){
    for (int i = 0; i < mInitSize; i++) {
        DBConn *dbConn = new DBConn(this);
        if (!dbConn->connect()) {
            delete dbConn;
            throw 1;
        }
        mFreeList.emplace_back(dbConn);
    }
    thread t(std::bind(&DBPool::scannerConnectionTask,this));
    t.detach();
}

DBPool::~DBPool() {
    lock_guard<mutex> lock(mMutex);
    mAbortRequest = true;
    mCondVar.notify_all();

    for(const auto& it : mFreeList){
        delete it;
    }
}


std::shared_ptr<DBConn> DBPool::getDbConn(int timeoutMs) {
    unique_lock<mutex> lock(mMutex);
    if (mAbortRequest) {
        printf("have aboort\n");
        return nullptr;
    }
    if (mFreeList.empty()) {
        //第一步先检测当前连接数量是否达到最大的连接数量
        if (mDbCurConnCnt >= mDbMaxConnCnt) {
            if (timeoutMs <= 0) {
                printf("wait ms:%d\n", timeoutMs);
                mCondVar.wait(lock);
                mCondVar.wait(lock, [this] {
                    printf("wait:%d, size:%lu\n", waitCount++, mFreeList.size());
                    return (!mFreeList.empty()) || mAbortRequest;
                });
            } else {
                mCondVar.wait_for(lock, chrono::milliseconds(timeoutMs), [this] {
                    printf("wait_for:%d, size:%lu\n", waitCount++, mFreeList.size());
                    return (!mFreeList.empty()) || mAbortRequest;
                });
                if (mFreeList.empty()) {
                    return nullptr;
                }
            }
            if (mAbortRequest) {
                printf("have aboort\n");
                return nullptr;
            }
        } else {  //还没到最大连接 创建连接
            DBConn* dbConn = new DBConn(this);
            if(!dbConn->connect()){
                printf("Init DBConnecton failed\n\n");
                delete dbConn;
                return nullptr;
            }else{
                mFreeList.emplace_back(dbConn);
                mDbCurConnCnt++;
            }
        }
    }
    auto dbConn = mFreeList.front();
    mFreeList.pop_front();

    shared_ptr<DBConn> sp(dbConn,std::bind(&DBPool::releaseConn,this,placeholders::_1));
    return sp;
}

void DBPool::releaseConn(DBConn* dbConn) {
    if(!dbConn){
        printf("dbConn is null\n");
        return;
    }
    {
        unique_lock<mutex> lock(mutex);
        dbConn->refreshAliveTime();
        mFreeList.push_back(dbConn);
        mCondVar.notify_all();
    }
}

void DBPool::scannerConnectionTask() {
    for(;;){
        this_thread::sleep_for(chrono::seconds(mMaxIdleTime));
        //扫描整个队列，释放多余连接
        unique_lock<mutex> lock(mMutex);
        while(mFreeList.size() > mInitSize){
            cout << "start  scanner" << endl;
            DBConn *p = mFreeList.front();
            if(p->getAliveTime() >= mMaxIdleTime){
                mFreeList.pop_front();
                mDbCurConnCnt--;
                delete p;
            }else{
                break;
            }
        }
    }
}
