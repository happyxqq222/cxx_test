//
// Created by Administrator on 2024/6/3.
//

#include "SendNode.h"

SendNode::SendNode(const std::shared_ptr<ByteBuffer> &buf, short msgId) :buf_(buf) , msgId_(msgId) {
}

std::shared_ptr<ByteBuffer> & SendNode::getBuf() {
    return buf_;
}

short SendNode::getMsgId() const {
    return msgId_;
}
