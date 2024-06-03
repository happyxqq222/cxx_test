//
// Created by Administrator on 2024/6/3.
//

#include "LogicSystem.h"

#include <json/reader.h>

using namespace std;

LogicSystem::LogicSystem() : msgQueue_(2048) ,stopFlag_(false){
    registerCallbacks();
    workerThread_ = std::thread(&LogicSystem::dealMsg, this);
}


bool LogicSystem::postMsgToQueue(const std::shared_ptr<LogicNode>& logiNode) {
    return msgQueue_.tryPush(logiNode);
}

LogicSystem::~LogicSystem() {
    stopFlag_ = true;
    std::shared_ptr<LogicNode> logicNode = nullptr;
    msgQueue_.push(logicNode);
    if(workerThread_.joinable()) {
        workerThread_.join();
    }
}

void LogicSystem::dealMsg() {
    for(;;) {
        shared_ptr<LogicNode> curNode;
        msgQueue_.take(curNode);
        if(stopFlag_) {
            break;
        }
        if(curNode == nullptr) {
            continue;
        }
        FunCallbackMap::const_accessor  acc;
        if(funCallbacks_.find(acc,curNode->getMsgId())) {
            acc->second(curNode->getSession(),curNode->getMsgId(),curNode->getMsg());
        }
    }
}

void LogicSystem::registerCallbacks() {
    auto fun = std::bind(&LogicSystem::helloWorldCallback,this,
        std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
    funCallbacks_.insert({MSG_HELLO_WORLD,fun});
}


void LogicSystem::helloWorldCallback(std::shared_ptr<Session> session,  short msgId,
    const std::string &msgData) {
    Json::Reader reader;
    Json::Value root;
    reader.parse(msgData,root);
    std::cout << "recevie msg id  is " << root["id"].asInt() << " msg data is "
    << root["data"].asString() << endl;
    root["data"] = "server has received msg, msg data is " + root["data"].asString();
    std::string return_str = root.toStyledString();
    session->send(return_str, root["id"].asInt());
}