//
// Created by Administrator on 2024-02-25.
//

#ifndef CPP_SERVER_TOOL_COMMONCONNECTIONPOOL_H
#define CPP_SERVER_TOOL_COMMONCONNECTIONPOOL_H
#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <functional>
#include <thread>
#include <fstream>
#include <condition_variable>
#include "Connection.h"

using namespace std::literals;

class ConnectionPool {
public:
    static ConnectionPool* getConnectionPool();
    //get available connection
    std::shared_ptr<Connection> getConnection();
    ~ConnectionPool();
private:
    ConnectionPool();  //单例#构造函数私有化
    //load config from config file
    bool loadConfigFile();
    //put connection into connection pool
    void recyleConnection(Connection* conn);
    //for produce new connection in a new connection
    void produceConnectionTask();
    void scanConectionTask();
private:
    std::string ip;
    uint32_t port;
    std::string username;
    std::string password;
    std::string dbname;
    uint32_t initSize;   //db connection initialize size
    uint32_t maxSize;    //db connnection maximum size
    uint32_t maxIdleTime;  //db connection max idle time
    uint32_t connectionTimeout;

    std::queue<Connection*> _connectionQueue; //this queue for store connection
    std::mutex _queueMutex;   //
    std::atomic_int _connectionCnt;
    std::condition_variable cv;  // its use for communicate between producer and consumer
    std::thread producerThread;
    std::thread scannerThread;

    struct FileStreamHelper{
        FileStreamHelper(const FileStreamHelper& helper) = delete;
        FileStreamHelper& operator=(const FileStreamHelper& helper) = delete;
        FileStreamHelper(std::ifstream& _fstream): fstream(_fstream){
        }
        ~FileStreamHelper(){
            fstream.close();
        }
    private:
        std::ifstream& fstream;
    };
};


#endif //CPP_SERVER_TOOL_COMMONCONNECTIONPOOL_H
