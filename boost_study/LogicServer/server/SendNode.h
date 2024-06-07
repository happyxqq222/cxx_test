//
// Created by Administrator on 2024/6/3.
//

#ifndef SENDNODE_H
#define SENDNODE_H
#include <memory>

#include "common/ByteBuffer.h"


class SendNode {

public:
    SendNode(const std::shared_ptr<ByteBuffer>& buf, short msgId);
    std::shared_ptr<ByteBuffer> &getBuf();
    short getMsgId() const;
private:
    std::shared_ptr<ByteBuffer> buf_;
    short msgId_;
};



#endif //SENDNODE_H
