#include "EncryptionTransfer/handler/MessageDecode.h"
#include "EncryptionTransfer/codec/Message.pb.h"
#include "EncryptionTransfer/net/UserUtil.h"
#include "EncryptionTransfer/handler/MessageHandler.h"

#include <muduo/base/Logging.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/Timestamp.h>

void MessageDecode::parseMessage(const muduo::net::TcpConnectionPtr& connPtr,
                            muduo::net::Buffer* buffer,
                            muduo::Timestamp timestamp) {
    if (readStatus == ReadStatus::Header) {
        if (buffer->readableBytes() >= 4) {
            contentLength = buffer->readInt32();
            readStatus = ReadStatus::Conentet;
        }
    }
    if (readStatus == ReadStatus::Conentet) {
        if (buffer->readableBytes() >= contentLength) {
            std::string content = buffer->retrieveAsString(contentLength);
            readStatus = ReadStatus::Header;
            ResponseMsg responseMsg;
            responseMsg.ParseFromString(content);
            MessageHandler msgHandler(rsaUtil_);
            msgHandler.onMessage(connPtr, responseMsg);
        }
    }
}
void MessageDecode::onClose(const muduo::net::TcpConnectionPtr& connPtr) {
    LOG_INFO << connPtr->peerAddress().toIp() << " 关闭连接";
}