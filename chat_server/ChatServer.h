//
// Created by Administrator on 2024-04-02.
//

#ifndef CHAT_SERVER_CHATSERVER_H
#define CHAT_SERVER_CHATSERVER_H

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/Callbacks.h>
#include <functional>
#include <memory>
#include <string>

using muduo::net::TcpConnection;
using muduo::net::TcpServer;
using muduo::net::EventLoop;
using muduo::net::InetAddress;
using muduo::net::TcpConnectionPtr;
using muduo::net::Buffer;
using muduo::Timestamp;

//聊天服务器的主类
class ChatServer {
public:
    ChatServer(EventLoop *loop,
               const InetAddress &listAddr,
               const std::string &nameArg);

    void startServer();

private:
    void onConnection(const TcpConnectionPtr &);
    void onMessage(const TcpConnectionPtr&,
                   Buffer*,
                   Timestamp);

    TcpServer mServer;   //组合的muduo库 实现服务器的类对象
    EventLoop *mLoop;    //指向事件循环的指针
};


#endif //CHAT_SERVER_CHATSERVER_H
