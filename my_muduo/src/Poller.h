//
// Created by Administrator on 2024-03-25.
//

#ifndef MY_MUDUO_POLLER_H
#define MY_MUDUO_POLLER_H


#include <vector>
#include <unordered_map>

#include "noncopyable.h"
#include "Timestamp.h"

class Channel;

class EventLoop;

//muduo库中多路事件分发器的核心IO复用模块
class Poller : noncopyable {
public:
    using ChannelList = std::vector<Channel *>;

    Poller(EventLoop *loop);

    virtual ~Poller();

    virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;

    virtual void updateChannel(Channel *pChannel) = 0;

    virtual void removeChannel(Channel *pChannel) = 0;

    //判断参数channel是否在当前Pller当中
    bool hasChannel(Channel *pChannel) const;

    //EventLoop可以通过该接口获取默认的IO复用的具体实现
    static Poller* newDefaultPooler(EventLoop *loop);
protected:
    //map的key: socketfd value: socketfd所属的channel通道类型
    using ChannelMap = std::unordered_map<int, Channel *>;
    ChannelMap channels_;
private:
    EventLoop *ownerLoop_;

};


#endif //MY_MUDUO_POLLER_H
