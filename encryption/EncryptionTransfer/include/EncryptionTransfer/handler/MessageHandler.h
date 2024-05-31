#pragma once

#include <muduo/net/Callbacks.h>
#include "EncryptionTransfer/codec/Message.pb.h"

class MessageHandler {
public:
    void onMessage(const muduo::net::TcpConnectionPtr& connPtr, const RequestMsg& reqMsg);
private:
    void onPing(const muduo::net::TcpConnectionPtr& connPtr, const RequestMsg& reqMsg);
    void onSecretAgree(const muduo::net::TcpConnectionPtr& connPtr, const RequestMsg& reqMsg);
    void onSecretResult(const muduo::net::TcpConnectionPtr& connPtr, const RequestMsg& reqMsg);
};