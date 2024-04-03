//
// Created by Administrator on 2024-04-03.
//

#include "ChatService.h"
#include "public.hpp"

#include <muduo/base/Logging.h>


using namespace std;

ChatService *ChatService::instance() {
    static ChatService service;
    return &service;
}

//处理登录业务
void ChatService::login(const TcpConnectionPtr &conn, Json::Value &json, Timestamp time) {
    LOG_INFO << "do login service!!!";
}

//处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn, Json::Value &json, Timestamp time) {
    LOG_INFO << "do reg service!!!";
}

//注册消息和对应的handler回调操作
ChatService::ChatService() {
    mMsgHandlerMap.insert({static_cast<int>(LOGIN_MSG),std::bind(&ChatService::login,this,placeholders::_1,placeholders::_2,placeholders::_3)});
    mMsgHandlerMap.insert({static_cast<int>(REG_MSG),std::bind(&ChatService::login,this,placeholders::_1,placeholders::_2,placeholders::_3)});
}

optional<MsgHandler> ChatService::getHandler(int msgId) {
    auto it = mMsgHandlerMap.find(msgId);
    if(it == mMsgHandlerMap.end()){
        LOG_ERROR << "msgid:" << msgId << "cant not find handler!";
        return nullopt;
    }else{
        return mMsgHandlerMap[msgId];
    }

}
