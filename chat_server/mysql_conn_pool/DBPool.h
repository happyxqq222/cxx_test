//
// Created by Administrator on 2024-04-04.
//

#ifndef CHAT_SERVER_DBPOOL_H
#define CHAT_SERVER_DBPOOL_H

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <map>
#include <stdint.h>
#include <mysql/mysql.h>
#include <string>
#include <string_view>
#include <memory>
#include <queue>
#include <list>
#include <thread>

#include "DBConn.h"



class DBPool {
public:
    DBPool() {}
    DBPool(std::string_view poolName,std::string_view dbServerIp,
           uint16_t dbServerPort,std::string_view userName,
           std::string_view password,std::string_view dbName,
           int maxConnCnt);
    virtual ~DBPool();
    std::shared_ptr<DBConn> getDbConn(int timeoutMs = 0);
    void releaseConn(DBConn *dbConn);  //归还连接
    void scannerConnectionTask();

    std::string_view getPoolName(){return mPoolName;}
    std::string_view getDbServerIp(){return mDbServerIp;}
    uint16_t getDbServerPort(){return mDbServerPort;}
    std::string_view getUsername(){return mUsername;}
    std::string_view getPassword(){return mPassword;}
    std::string_view getDbName(){return mDbName;}
    uint32_t getDbConnectionCount(){return mFreeList.size();}
private:
    std::string mPoolName; //连接池名称
    std::string mDbServerIp;  //数据库ip
    std::uint16_t mDbServerPort = 3306; //数据库端口
    std::string mUsername;  //用户名
    std::string mPassword;  //密码
    std::string mDbName;    //db名称
    int mDbCurConnCnt;     //当前启用的连接数量
    int mDbMaxConnCnt;     //最大连接数量
    int mInitSize;         //初始化大小
    int mMaxIdleTime;      //连接池最大空闲时间


    std::list<DBConn*> mFreeList;  //空闲连接
    std::mutex mMutex;
    std::condition_variable mCondVar;
    bool mAbortRequest = false;
    int waitCount = 0;
};


#endif //CHAT_SERVER_DBPOOL_H
