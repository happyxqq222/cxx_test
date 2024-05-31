
#include "EncryptionTransfer/handler/MessageHandler.h"
#include "EncryptionTransfer/codec/Message.pb.h"
#include "EncryptionTransfer/net/UserUtil.h"
#include "EncryptionTransfer/tool/AESUtils.h"
#include "EncryptionTransfer/tool/NetUtils.h"
#include "EncryptionTransfer/tool/RSAUtils.h"

#include <json/reader.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <boost/asio/thread_pool.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

void MessageHandler::onMessage(const muduo::net::TcpConnectionPtr& connPtr,
                               const RequestMsg& reqMsg) {
    if(reqMsg.cmdtype() == 0) {
        onPing(connPtr, reqMsg);
    } else if (reqMsg.cmdtype() == 1) {
        onSecretAgree(connPtr, reqMsg);
    } else if(reqMsg.cmdtype() == 2) {
        onSecretResult(connPtr, reqMsg);
    }
}

void MessageHandler::onPing(const muduo::net::TcpConnectionPtr& connPtr, const RequestMsg& reqMsg) {
    LOG_INFO << "收到心跳包" ;
    UserUtil::getInstance()->addOrUpdateConnTime(connPtr,muduo::Timestamp::now().secondsSinceEpoch());
    ResponseMsg responseMsg;
    responseMsg.set_timestamp(muduo::Timestamp::now().secondsSinceEpoch());
    responseMsg.set_cmdtype(0);
    NetUtils::sendMsg(connPtr, responseMsg);
}

void MessageHandler::onSecretAgree(const muduo::net::TcpConnectionPtr& connPtr, const RequestMsg& reqMsg) {
    Json::Reader reader;
    LOG_INFO << "收到密钥协商" ;
    //对签名进行校验 ->使用公钥进行验签名
    //加载公钥
    RSAUtil rsaUtil;
    rsaUtil.load(reqMsg.data(), RsaKeyType::PUBLIC);
    //使用传过来的公钥进行验证签名
    bool verifyResult = rsaUtil.signVerifyWithPublicKey(reqMsg.data(), reqMsg.sign());
    ResponseMsg responseMsg;
    responseMsg.set_cmdtype(1);

    if(verifyResult) {
        string key = AESUtils::generateRandomStr(16);
        string iv = AESUtils::generateRandomStr(16);
        shared_ptr<AESUtils> aesUtils = make_shared<AESUtils>(std::move(key),std::move(iv));
        UserUtil::getInstance()->addAes(connPtr, aesUtils);
        LOG_INFO << "密钥:" << aesUtils->getSecretKey();
        LOG_INFO << "向量:" << aesUtils->getIv();
        string data;
        data.append(aesUtils->getSecretKey()).append(aesUtils->getIv());
        responseMsg.set_data(rsaUtil.encryptWithRsa(data));
    }
    responseMsg.set_status(verifyResult);
    NetUtils::sendMsg(connPtr, responseMsg);
}

void MessageHandler::onSecretResult(const muduo::net::TcpConnectionPtr& connPtr, const RequestMsg& reqMsg) {
    shared_ptr<AESUtils> aesUtil = UserUtil::getInstance()->getAes(connPtr);
    LOG_INFO << "密钥:" << aesUtil->getSecretKey();
    LOG_INFO << "向量:" << aesUtil->getIv();

/*     string encryptText =  aesUtil->encrypt("{isSuccess: 1}");
    for(int i =0;i<reqMsg.data().size();i++){
        cout << static_cast<uint32_t>((uint8_t)(reqMsg.data()[i])) << " ";
    }
    cout << endl;

    for(int i =0;i<encryptText.size();i++){
        cout << static_cast<uint32_t>((uint8_t)(encryptText[i])) << " ";
    }
    cout << endl;
    LOG_INFO << aesUtil->decrypt(encryptText); */
    try{
        LOG_INFO << "协商结果:" << aesUtil->decrypt(reqMsg.data());
    }catch(std::runtime_error& err){
        LOG_INFO << err.what() ;
    }
    
}