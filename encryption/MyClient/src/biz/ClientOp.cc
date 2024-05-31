
#include "EncryptionTransfer/biz/ClientOp.h"
#include "EncryptionTransfer/codec/Codec.h"
#include "EncryptionTransfer/codec/CodecFactory.h"
#include "EncryptionTransfer/codec/RequestCodec.h"
#include "EncryptionTransfer/codec/RequestFactory.h"
#include "EncryptionTransfer/net/Client.h"
#include "EncryptionTransfer/tool/RSAUtils.h"
#include "EncryptionTransfer/tool/Types.h"

#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <fstream>
#include <memory>
#include <string>
#include <muduo/base/Logging.h>


using namespace std;



ClientOp::ClientOp(RSAUtil& rsaUtil,Client& client) :rsaUtil_(rsaUtil),client_(client) {
    Json::Value root;
    Json::Reader reader;
    string json;
    {
        ifstream file("server.json",ios_base::out);
        unique_ptr<ifstream,default_delete<ifstream>> fs(&file);
        if(file.is_open()) {
            std::string line;
            while(std::getline(file,line)) {
                json.append(line);
            }
        }
    }
    reader.parse(json,root);
    serverId_ = root["ServerID"].asString();
    clientId_ = root["ClientID"].asString();
    LOG_INFO << "serverId:" << serverId_ << " clientId:" << clientId_; 
}


//密钥协商
bool ClientOp::seckeyArgree() {
    //1.初始化序列化数据
    RequestInfo reqInfo;
    reqInfo.clientId = clientId_;
    reqInfo.serverId = serverId_;
    reqInfo.cmdType  = 1;
    reqInfo.data = rsaUtil_.getPublicKeyStr();       //非对称加密的公钥
    reqInfo.sign = rsaUtil_.signWithPrivateKey(reqInfo.data);       //公钥签名
    unique_ptr<CodecFactory> factory = make_unique<RequestFactory>(reqInfo);
    shared_ptr<Codec> codec =  factory->createCodec();
    //得到序列化之后的数据就可以发送了
    string encStr = codec->encodeMsg();
    client_.send(encStr);
    return false;
}

//密钥校验
bool ClientOp::seckeyCheck() {
    return false;
}

//密钥注销
void ClientOp::seckeyLogout() {
}