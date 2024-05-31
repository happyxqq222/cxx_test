#pragma once
#include <muduo/net/Callbacks.h>
#include <muduo/net/TcpConnection.h>
#include "EncryptionTransfer/codec/Message.pb.h"

class NetUtils {
public:
    static void sendResponseMsg(const muduo::net::TcpConnectionPtr& connPtr, const ResponseMsg& responseMsg);
    static void sendRequestMsg(const muduo::net::TcpConnectionPtr& connPtr, const RequestMsg& requestMsg);
};