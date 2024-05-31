
#include "EncryptionTransfer/handler/MessageHandler.h"
#include "EncryptionTransfer/codec/Message.pb.h"
#include "EncryptionTransfer/net/Client.h"
#include "EncryptionTransfer/tool/AESUtils.h"
#include "EncryptionTransfer/tool/NetUtils.h"

#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <boost/any.hpp>
#include <boost/any/bad_any_cast.hpp>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

MessageHandler::MessageHandler(RSAUtil& rsaUtil) :rsaUtil_(rsaUtil) {

}

void MessageHandler::onMessage(const muduo::net::TcpConnectionPtr& connPtr,
                               const ResponseMsg& responseMsg) {
    if(responseMsg.cmdtype() == 0) {
        onPing(connPtr, responseMsg);
    } else if(responseMsg.cmdtype() == 1) {
        onSecretKeyAgree(connPtr, responseMsg);
    }
}

void MessageHandler::onPing(const muduo::net::TcpConnectionPtr& connPtr, const ResponseMsg& responseMsg) {
//    LOG_INFO<< "客户端收到心跳";
/*     RequestMsg requestMsg;
    requestMsg.set_timestamp(muduo::Timestamp::now().secondsSinceEpoch());
    requestMsg.set_cmdtype(0);
    NetUtils::sendRequestMsg(connPtr, requestMsg); */
}

void MessageHandler::onSecretKeyAgree(const muduo::net::TcpConnectionPtr& connPtr, const ResponseMsg& responseMsg) {
    if(!responseMsg.status()) {
        LOG_ERROR << "密钥协商失败";
        return;
    }
    Client* client =  boost::any_cast<Client*>(connPtr->getContext());
    string plainData = rsaUtil_.decryptWithRsa(responseMsg.data());
    string key (plainData.data(),plainData.data()+16);
    string iv (plainData.data()+16);

    std::unique_ptr<AESUtils> aesUtilUptr = make_unique<AESUtils>(std::move(key),std::move(iv));
    LOG_INFO << "密钥:" << aesUtilUptr->getSecretKey();
    LOG_INFO << "向量:" << aesUtilUptr->getIv();
    client->setAesUtil(aesUtilUptr);
    AESUtils* aesUtil = client->getAesUtil();
    //告诉服务端收到密钥
    RequestMsg requestMsg;
    requestMsg.set_cmdtype(2);
    requestMsg.set_data(aesUtil->encrypt("{isSuccess: 1}"));

    for(int i =0;i<requestMsg.data().size();i++){
        cout << static_cast<uint32_t>((uint8_t)(requestMsg.data()[i])) << " ";
    }
    cout << endl;
    
    LOG_INFO << aesUtil->decrypt(requestMsg.data());

    NetUtils::sendRequestMsg(connPtr, requestMsg);
}