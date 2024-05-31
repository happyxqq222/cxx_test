#pragma once

#include <boost/core/noncopyable.hpp>
#include <memory>
#include <string>
#include "EncryptionTransfer/net/Client.h"
#include "EncryptionTransfer/tool/RSAUtils.h"

class ClientOp : boost::noncopyable{
public:
    ClientOp(RSAUtil& rsaUtil,Client& client);
    ~ClientOp() = default;

    //密钥协商
    bool seckeyArgree();
    //密钥校验
    bool seckeyCheck();
    //密钥注销
    void seckeyLogout();

private:
    std::string clientId_;
    std::string serverId_;
    RSAUtil& rsaUtil_;
    Client& client_;
};