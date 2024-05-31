#pragma once
#include <muduo/net/TcpConnection.h>
#include "EncryptionTransfer/codec/Message.pb.h"

class NetUtils {
public:
    static void sendMsg(const muduo::net::TcpConnectionPtr& connPtr, const ResponseMsg& responseMsg);
};