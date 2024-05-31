
#include "EncryptionTransfer/net/Client.h"
#include "EncryptionTransfer/codec/Message.pb.h"
#include "EncryptionTransfer/handler/MessageDecode.h"
#include "EncryptionTransfer/tool/NetUtils.h"
#include "EncryptionTransfer/tool/RSAUtils.h"

#include <muduo/base/Logging.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpClient.h>
#include <algorithm>
#include <functional>
#include <memory>
#include <latch>


using namespace std;
using namespace muduo::net;
using namespace muduo;

Client::Client(RSAUtil& rsaUtil) :l(1),messageDecode_(make_unique<MessageDecode>(rsaUtil)),rsaUtil_(rsaUtil) {}

void Client::clientStart() {
    clientLoop_ = make_unique<EventLoop>();
    InetAddress serverAddr("192.168.17.128", 8888);
    tcpClient_ = make_unique<TcpClient>(clientLoop_.get(), serverAddr, "client");
    tcpClient_->setConnectionCallback(
        std::bind(&Client::onConnection, this, std::placeholders::_1));
    tcpClient_->setMessageCallback(std::bind(&Client::onMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             std::placeholders::_3));
    tcpClient_->connect();
    clientLoop_->loop();
}

void Client::start() {
    clientThread_ = thread(std::bind(&Client::clientStart, this)); //启动一个线程来处理客户端连接
    l.wait();
}

void Client::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
    messageDecode_->parseMessage(connPtr_, buf, time);
}

void Client::onConnection(const TcpConnectionPtr& connPtr) {
    if (connPtr->connected()) {
        connPtr->setContext(this);
        l.count_down();
        LOG_INFO << "建立连接";
        connPtr_ = connPtr;
        clientLoop_->runEvery(5,std::bind(&Client::sendHeartBeat,this));
        //connPtr->setConnectionCallback(std::bind(&Client::onConnection,this,std::placeholders::_1));
        connPtr->setCloseCallback(std::bind(&Client::onClose,this,std::placeholders::_1));
        //connPtr->setMessageCallback(std::bind(&Client::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    }
}

void Client::sendHeartBeat() {
    if (connPtr_->connected()) {
        RequestMsg requestMsg;
        requestMsg.set_cmdtype(0);
        requestMsg.set_timestamp(Timestamp::now().secondsSinceEpoch());
        NetUtils::sendRequestMsg(connPtr_, requestMsg);
    }
}

void Client::onClose(const TcpConnectionPtr& conn) {

}

bool Client::isConnected() {
    return connPtr_->connected();
}

void Client::send(const string& content) {
    if(connPtr_ && connPtr_->connected()) {
        Buffer buffer;
        buffer.appendInt32(content.size());
        buffer.append(content);
        connPtr_->send(&buffer);
    }
}

void Client::setAesUtil(std::unique_ptr<AESUtils>& aesUtil) {
    aesUtil_ = std::move(aesUtil);
}

AESUtils* Client::getAesUtil() {
    return aesUtil_.get();
}

Client::~Client() {
    if(clientThread_.joinable()) {
        clientThread_.join();
    }
}