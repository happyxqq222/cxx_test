//
// Created by Administrator on 2024-03-25.
//

#include "Poller.h"
#include "Channel.h"

Poller::Poller(EventLoop *loop) : ownerLoop_(loop) {
}

Poller::~Poller() {

}

bool Poller::hasChannel(Channel *pChannel) const {
    auto findIt = channels_.find(pChannel->fd());
    return findIt != channels_.end() && findIt->second == pChannel;
}
