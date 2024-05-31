#include "EncryptionTransfer/tool/NetUtils.h"
#include <muduo/net/Buffer.h>
#include <string>


void NetUtils::sendMsg(const muduo::net::TcpConnectionPtr& connPtr, const ResponseMsg& responseMsg) {
    if(connPtr != nullptr) {
        std::string serializeMsg =  responseMsg.SerializeAsString();
        int contentLen = serializeMsg.size();
        muduo::net::Buffer buffer;
        buffer.appendInt32(contentLen);
        buffer.append(serializeMsg);
        connPtr->send(&buffer);
    }

}