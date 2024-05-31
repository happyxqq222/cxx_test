
#include "EncryptionTransfer/net/Server.h"
#include "EncryptionTransfer/handler/MessageDecode.h"
#include "EncryptionTransfer/net/UserUtil.h"
#include "EncryptionTransfer/tool/Types.h"

#include <json/reader.h>
#include <json/value.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoopThread.h>
#include <pthread.h>
#include <fstream>
#include <functional>
#include <ios>
#include <memory>
#include <sstream>
#include <json/json.h>

using namespace muduo;
using namespace muduo::net;
using namespace std;

Server::Server()
        : timerLoop(std::bind(&Server::addHeaderbeatCheck, this, std::placeholders::_1),"timerLoop")
        , idIndex(0) {
    eventLoop = std::make_unique<EventLoop>();
    connectionInOut = std::make_unique<ConnectionInOut>();
    InetAddress inetAddr(8888);
    tcpServer =
        std::make_unique<TcpServer>(eventLoop.get(), inetAddr, "myserver", TcpServer::kReusePort);
    tcpServer->setThreadNum(3);
    tcpServer->setConnectionCallback(std::bind(&Server::onConnection, this, std::placeholders::_1));
    userUtil = UserUtil::getInstance();
}

void Server::parseConfig() {
    ifstream fs("server.json",ios_base::in|ios_base::out);
    if(!fs.is_open()) {
        LOG_FATAL << "open server.json error";
        return;
    }
    unique_ptr<ifstream> fsUptr(&fs);
    Json::Reader reader;
    Json::Value root;
    stringstream ss;
    ss << "json:" << fs.rdbuf();
    LOG_INFO << ss.str();
    reader.parse(fs,root);
    string serverId =  root["ServerID"].asString();
    LOG_INFO << "serverId:" << serverId;
}

void Server::startServer() {
    parseConfig();
    timerLoop.startLoop();
    tcpServer->start();
    eventLoop->loop();
}

void Server::checkHeartbeat() {
    LOG_INFO << "start check heartbeat";
    time_t now = Timestamp::now().secondsSinceEpoch();
    ConnTimeMap connTimeMap = userUtil->getConnTimeMap();
    for (const auto& item : connTimeMap) {
        if (now - item.second > 10) {
            LOG_INFO << "踢掉: ";
            item.first->shutdown();
            userUtil->removeConnTime(item.first);
            userUtil->removeMessageCodec(item.first);
            userUtil->removeUserByConnPtr(item.first);
        }
    }
    LOG_INFO << "end check heartbeat";
}

void Server::addHeaderbeatCheck(muduo::net::EventLoop* loop) {
    loop->runEvery(8, std::bind(&Server::checkHeartbeat, this));
}

void Server::onConnection(const muduo::net::TcpConnectionPtr& connPtr) {
    connPtr->setMessageCallback(std::bind(&MessageDecode::onMessage,
                                          userUtil->getMessageCodec(connPtr),
                                          std::placeholders::_1,
                                          std::placeholders::_2,
                                          std::placeholders::_3));

    connPtr->setCloseCallback(std::bind(
        &Server::onClose, this, std::placeholders::_1));
    userUtil->addOrUpdateConnTime(connPtr, Timestamp::now().secondsSinceEpoch());
    userUtil->addUser(idIndex++,connPtr);
}

void Server::onClose(const muduo::net::TcpConnectionPtr& connPtr) {
    LOG_INFO << connPtr->peerAddress().toIp() << " 关闭连接";
    userUtil->removeConnTime(connPtr);
    userUtil->removeMessageCodec(connPtr);
    userUtil->removeUserByConnPtr(connPtr);
}