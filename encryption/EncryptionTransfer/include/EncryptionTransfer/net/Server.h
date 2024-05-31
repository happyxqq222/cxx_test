#pragma once

#include "EncryptionTransfer/net/ConnectionInOut.h"

#include <atomic>
#include <memory>
#include <muduo/base/FileUtil.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/net/TcpServer.h>
#include <boost/core/noncopyable.hpp>
#include <oneapi/tbb/concurrent_hash_map.h>
#include <tbb/concurrent_hash_map.h>
#include <boost/thread/thread_pool.hpp>


class UserUtil;

class Server : boost::noncopyable{

public:
   explicit Server();
   void startServer();
   void checkHeartbeat();
   void addHeaderbeatCheck(muduo::net::EventLoop *loop);
   void onConnection(const muduo::net::TcpConnectionPtr& connPtr);
   void onClose(const muduo::net::TcpConnectionPtr& connPtr);

private:
   void parseConfig();

private:
   std::unique_ptr<muduo::net::TcpServer> tcpServer;
   std::unique_ptr<muduo::net::EventLoop> eventLoop;
   std::unique_ptr<ConnectionInOut> connectionInOut;
   muduo::net::EventLoopThread timerLoop;
   std::shared_ptr<UserUtil> userUtil;
   std::atomic_int idIndex;
};