//
// Created by Administrator on 2024-03-25.
//

#include "Pooler.h"
#include "Channel.h"

Pooler::Pooler(EventLoop *loop) :ownerLoop_(loop) {
}

Pooler::~Pooler() {

}

bool Pooler::hasChannel(Channel *pChannel) const {
    auto findIt = channels_.find(pChannel->fd());
    return findIt != channels_.end() && findIt->second == pChannel;
}
