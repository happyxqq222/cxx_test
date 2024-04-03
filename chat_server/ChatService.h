//
// Created by Administrator on 2024-04-03.
//

#ifndef CHAT_SERVER_CHATSERVICE_H
#define CHAT_SERVER_CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <json/json.h>
#include <unordered_map>
#include <optional>

#include "public.hpp"

using muduo::net::TcpConnectionPtr;
using muduo::Timestamp;

using MsgHandler = std::function<void(const TcpConnectionPtr &, Json::Value &, Timestamp)>;

//聊天服务器类
class ChatService {
public:
    static ChatService* instance();

    void login(const TcpConnectionPtr &conn, Json::Value &json, Timestamp time);

    void reg(const TcpConnectionPtr &conn, Json::Value &json, Timestamp time);

    //获取消息对应的处理器
    std::optional<MsgHandler> getHandler(int msgid);

private:
    ChatService();
    std::unordered_map<int,MsgHandler> mMsgHandlerMap;
};


#endif //CHAT_SERVER_CHATSERVICE_H
