//
// Created by Administrator on 2024-02-25.
//

#ifndef CPP_SERVER_TOOL_COMMONCONNECTIONPOOL_H
#define CPP_SERVER_TOOL_COMMONCONNECTIONPOOL_H
#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include "Connection.h"


class ConnectionPool {
public:
    static ConnectionPool* getConnectionPool();
private:
    ConnectionPool();  //单例#构造函数私有化
    //从配置文件中加载配置
    bool loadConfigFile();
private:
    std::string _ip;
    uint16_t _port;
    std::string _username;
    std::string _password;
    uint32_t _initSize;   //初始连接量
    uint32_t _maxSize;    //连接池最大连接量
    uint32_t _maxIdleTime;  //连接池最大空闲时间
    uint32_t _connectionTimeout;   //连接池获取连接的超时时间

    std::queue<Connection*> _connectionQueue; //存储mysql连接的队列
    std::recursive_mutex _queueMutex;   //维护连接队列的线程互斥安全锁
};


#endif //CPP_SERVER_TOOL_COMMONCONNECTIONPOOL_H
