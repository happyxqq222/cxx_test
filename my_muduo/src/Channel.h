//
// Created by Administrator on 2024-03-24.
//

#ifndef MY_MUDUO_CHANNEL_H
#define MY_MUDUO_CHANNEL_H

#include <functional>
#include <memory>

#include "noncopyable.h"

class EventLoop;

class Timestamp;


class Channel : noncopyable {
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop *loop, int fd);

    ~Channel();

    void handleEvent(Timestamp receiveTime);

private:
    static const int kNoneEvent;
    static const int KReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;  //event loop
    const int fd_;  // fd, Pooler监听的对象
    int events_;   //注册fd感兴趣的事件
    int revents_;  //pooler返回的具体发生的事件
    int index_;
    std::weak_ptr<void> tie_;
    bool tied_;
};


#endif //MY_MUDUO_CHANNEL_H
