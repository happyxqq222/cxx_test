//
// Created by Administrator on 2024/6/3.
//

#include "LogicNode.h"

LogicNode::LogicNode(const std::shared_ptr<Session> &session, short msgId, const std::string &msg)
    : session_(session), msgId_(msgId), msg_(msg) {
}

LogicNode::LogicNode(const std::shared_ptr<Session> &session, short msgId, std::string &&msg)
    : session_(session), msgId_(msgId), msg_(std::move(msg)) {
}
