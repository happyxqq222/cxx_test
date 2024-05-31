#pragma once

#include "EncryptionTransfer/handler/MessageDecode.h"
#include "EncryptionTransfer/tool/AESUtils.h"
#include "EncryptionTransfer/tool/RSAUtils.h"

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpClient.h>
#include <boost/core/noncopyable.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <boost/thread/latch.hpp>

class Client  : boost::noncopyable{

public:
    Client(RSAUtil& rsaUtil);
    void start();
    bool isConnected();
    void send(const std::string& content);
    void setAesUtil(std::unique_ptr<AESUtils>& aesUtil);
    AESUtils* getAesUtil();
    ~Client();

private:
    void clientStart();
    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time);
    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    void sendHeartBeat();
    void onClose(const muduo::net::TcpConnectionPtr& conn);

private:
    std::unique_ptr<muduo::net::EventLoop> clientLoop_;
    std::unique_ptr<MessageDecode> messageDecode_;
    std::thread clientThread_;
    std::unique_ptr<muduo::net::TcpClient> tcpClient_;
    muduo::net::TcpConnectionPtr connPtr_;
    boost::latch l;
    RSAUtil& rsaUtil_;
    std::unique_ptr<AESUtils> aesUtil_;
};