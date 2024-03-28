//
// Created by Administrator on 2024-03-24.
//

#ifndef MY_MUDUO_EVENTLOOP_H
#define MY_MUDUO_EVENTLOOP_H
#include <functional>
#include <vector>
#include <atomic>
#include <memory>

#include "Timestamp.h"
#include "noncopyable.h"
#include "CurrentThread.h"

class Channel;
class Poller;

class EventLoop :noncopyable{

public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    Timestamp pollReturnTime() const{return pollReturnTime_;}

    void runInLoop(Functor cb);

    void queueInLoop(Functor cb);

    void wakeup();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    bool isInLoopThread() const{
        return threadId_ == CurrentThread::tid();
    }
private:
    void handleRead(Timestamp timestamp);
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;

    std::atomic_bool looping_; //
    std::atomic_bool quit_;   //

    const pid_t  threadId_;   //

    Timestamp pollReturnTime_;   //
    std::unique_ptr<Poller> poller_;

    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_;

    std::atomic_bool callingPendingFunctors_; //
    std::vector<Functor> pendingFunctors_;  //
    std::mutex mutex_;  //
};


#endif //MY_MUDUO_EVENTLOOP_H
