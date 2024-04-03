//
// Created by Administrator on 2024-04-02.
//


#include "ChatServer.h"
#include "ChatService.h"

#include <string>
#include <json/json.h>
#include <iostream>
#include <muduo/base/Logging.h>

using namespace Json;
using namespace std;
using namespace muduo;
using namespace muduo::net;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listAddr,
                       const std::string &nameArg)
        : mServer(loop, listAddr, nameArg), mLoop(loop) {

    mServer.setConnectionCallback(std::bind(&ChatServer::onConnection, this, placeholders::_1));

    mServer.setMessageCallback(
            std::bind(&ChatServer::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));
    mServer.setThreadNum(4);
}

void ChatServer::startServer() {
    mServer.start();
}

void ChatServer::onConnection(const TcpConnectionPtr & conn) {
    if(!conn->connected()){
        conn->shutdown();
    }
}

void ChatServer::onMessage(const TcpConnectionPtr & conn, Buffer * buffer, Timestamp timestamp) {
    string strBuf = buffer->retrieveAllAsString();
    Json::Reader reader;
    Value json;
    LOG_INFO << "msg:" << strBuf;
    if(reader.parse(strBuf, json, false)){
        Value msgId =  json.get("msgid", nullValue);
        if(msgId != nullValue){
            optional<MsgHandler>  msgHandler = ChatService::instance()->getHandler(msgId.asInt());
            if(msgHandler){
                (*msgHandler)(conn, json, timestamp);
            }
        }else{
            LOG_ERROR << "get msgid error :" << strBuf ;
        }

    }else{
        LOG_ERROR << "parse json error :" << strBuf ;
    }

}
