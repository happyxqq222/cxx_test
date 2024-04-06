//
// Created by Administrator on 2024-04-04.
//

#ifndef CHAT_SERVER_DBCONNECTIONPOOL_H
#define CHAT_SERVER_DBCONNECTIONPOOL_H

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

#include "DBConnection.h"



class DBConnectionPool {
public:
    DBConnectionPool() {}
    DBConnectionPool(std::string_view poolName, std::string_view dbServerIp,
                     uint16_t dbServerPort, std::string_view userName,
                     std::string_view password, std::string_view dbName,
                     int maxConnCnt);
    virtual ~DBConnectionPool();
    std::shared_ptr<DBConnection> getDbConn();
    void releaseConn(DBConnection *dbConn);  //归还连接
    void scannerConnectionTask();

    std::string_view getPoolName(){return _poolName;}
    std::string_view getDbServerIp(){return _dbServerIp;}
    uint16_t getDbServerPort(){return _dbServerPort;}
    std::string_view getUsername(){return username;}
    std::string_view getPassword(){return password;}
    std::string_view getDbName(){return dbName;}
    uint32_t getDbConnectionCount(){return _freeList.size();}
private:
    std::string _poolName; //连接池名称
    std::string _dbServerIp;  //数据库ip
    std::uint16_t _dbServerPort = 3306; //数据库端口
    std::string username;  //用户名
    std::string password;  //密码
    std::string dbName;    //db名称
    int _connectionCount;     //当前建立数据库连接的数量(被使用的和空闲的)
    int _maxConnectionCount;     //最大连接数量
    int _initSize;         //初始化大小
    int _maxIdleTime;      //连接池最大空闲时间


    std::list<DBConnection*> _freeList;  //空闲连接
    std::mutex _mutex;
    std::condition_variable _condVar;
    bool _abortRequest = false;
    int waitCount = 0;
    std::thread scannerTask;
};


#endif //CHAT_SERVER_DBCONNECTIONPOOL_H
