#pragma once

#include <muduo/net/Callbacks.h>
#include "EncryptionTransfer/codec/Message.pb.h"
#include "EncryptionTransfer/tool/RSAUtils.h"

class MessageHandler {
public:
    MessageHandler(RSAUtil& rsaUtil);
    void onMessage(const muduo::net::TcpConnectionPtr& connPtr, const ResponseMsg& responseMsg);
private:
    void onPing(const muduo::net::TcpConnectionPtr& connPtr, const ResponseMsg& responseMsg);
    void onSecretKeyAgree(const muduo::net::TcpConnectionPtr& connPtr, const ResponseMsg& responseMsg);
private:
    RSAUtil& rsaUtil_;
};