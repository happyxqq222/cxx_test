//
// Created by Administrator on 2024-03-25.
//

#ifndef MY_MUDUO_EPOLLPOLLER_H
#define MY_MUDUO_EPOLLPOLLER_H

#include "Poller.h"

#include <vector>
#include <sys/epoll.h>

/**
 * epoll的使用
 * epoll_create
 * epoll_ctl  add/mod/del
 * epoll_wait
 */
class EpollPoller : public Poller {
public:
    EpollPoller(EventLoop* loop);

    virtual ~EpollPoller() override;

    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;

    void updateChannel(Channel *pChannel) override;

    void removeChannel(Channel *pChannel) override;
private:
    static const int kInitEventListSize = 16;

    //填充活跃的连接
    void fillActiveChannels(int numEvents,ChannelList* activeChannels) const;

    //更新channel通道
    void update(int operation, Channel* channel);

    using EventList = std::vector<epoll_event>;
    int epollfd_;
    EventList  events_;
};


#endif //MY_MUDUO_EPOLLPOLLER_H
