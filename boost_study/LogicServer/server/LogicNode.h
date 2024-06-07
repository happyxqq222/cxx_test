//
// Created by Administrator on 2024/6/3.
//

#ifndef LOGICNODE_H
#define LOGICNODE_H
#include <memory>

#include "Session.h"


class LogicNode {

public:
    LogicNode(const std::shared_ptr<Session>& session, short msgId, const std::string& msg);
    LogicNode(const std::shared_ptr<Session>& session, short msgId, std::string&& msg);

    std::string& getMsg() {
        return msg_;
    }

    [[nodiscard]] short getMsgId() const {
        return msgId_;
    }

    std::shared_ptr<Session>& getSession() {
        return session_;
    }
private:
    std::shared_ptr<Session> session_;
    std::string msg_;
    short msgId_;
};



#endif //LOGICNODE_H
