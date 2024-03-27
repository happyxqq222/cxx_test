//
// Created by Administrator on 2024-03-25.
//
#include "Poller.h"
#include "EpollPoller.h"
#include <stdlib.h>

Poller *Poller::newDefaultPooler(EventLoop *loop) {
    if (::getenv("MUDUO_USE_POLL")) {
        return nullptr;
    } else {
        return new EpollPoller(loop);
    };
}