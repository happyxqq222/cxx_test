
#include <EncryptionTransfer/net/ConnectionInOut.h>
#include <EncryptionTransfer/net/UserUtil.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/TcpConnection.h>
#include <functional>
#include <string>
#include <EncryptionTransfer/codec/Message.pb.h>
#include <muduo/base/Logging.h>
#include <netinet/tcp.h>

using namespace muduo::net;
using namespace muduo;
using namespace std;

ConnectionInOut::ConnectionInOut(): readStatus(ReadStatus::Header),contentLength(-1) {
    
}

void ConnectionInOut::onConnection(const TcpConnectionPtr& connPtr) {
    connPtr->setCloseCallback(std::bind(&ConnectionInOut::onClose, this, std::placeholders::_1));
    connPtr->setMessageCallback(std::bind(&ConnectionInOut::onMessage, this, std::placeholders::_1,
                                          std::placeholders::_2, std::placeholders::_3));
    UserUtil::getInstance()->addOrUpdateConnTime(connPtr, Timestamp::now().secondsSinceEpoch());
}

void ConnectionInOut::onMessage(const TcpConnectionPtr& connPtr, Buffer* buffer, Timestamp timestamp) {
    if(readStatus == ReadStatus::Header) {
        if(buffer->readableBytes() >= 4) {
            contentLength = buffer->readInt32();
            readStatus = ReadStatus::Conentet;
            if(buffer->readableBytes() >= contentLength) {
                std::string content = buffer->retrieveAsString(contentLength);
                RequestMsg requestMsg ;
                requestMsg.ParseFromString(content);
            }
        }
    } else {
        if(buffer->readableBytes() >= contentLength) {
            std::string content = buffer->retrieveAsString(contentLength);
            readStatus = ReadStatus::Header;
        }
    }
}

void ConnectionInOut::onClose(const TcpConnectionPtr& connPtr) {
    LOG_INFO << connPtr->peerAddress().toIp() << " disconnect" ;
    UserUtilPtr userUtilPtr =  UserUtil::getInstance();
    userUtilPtr->removeUserByConnPtr(connPtr);
}