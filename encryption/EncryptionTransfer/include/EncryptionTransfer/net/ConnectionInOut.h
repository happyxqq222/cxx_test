#pragma once

#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>

class ConnectionInOut {
public:
    enum class ReadStatus{
        Header,
        Conentet
    };
    ConnectionInOut();
    void onConnection(const muduo::net::TcpConnectionPtr& connPtr);
    void onMessage(const muduo::net::TcpConnectionPtr& connPtr,
                   muduo::net::Buffer* buffer, muduo::Timestamp timestamp);
    void onClose(const muduo::net::TcpConnectionPtr& connPtr);

private:
    muduo::net::Buffer buffer;
    ReadStatus readStatus;
    int contentLength;
};