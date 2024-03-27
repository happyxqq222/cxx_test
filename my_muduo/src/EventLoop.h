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

    //开启时间循环
    void loop();

    //退出事件循环
    void quit();

    Timestamp pollReturnTime() const{return pollReturnTime_;}

    //在当前loop中执行 cb
    void runInLoop(Functor cb);

    //把cb放入队列中，唤醒loop所在的线程 执行cb
    void queueInLoop(Functor cb);

    //用来唤醒loop所在的线程的
    void wakeup();

    //EventLoop的方法 => Pooler的方法
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    //判断EventLoop对象是否在自己的线程里
    bool isInLoopThread() const{
        return threadId_ == CurrentThread::tid();
    }
private:
    void handleRead(Timestamp timestamp);
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;

    std::atomic_bool looping_; //原子操作 通过CAS实现的
    std::atomic_bool quit_;   //标识 退出loop循环

    const pid_t  threadId_;   //记录当前loop所在线程的id

    Timestamp pollReturnTime_;   //返回Poller返回发生事件的channels的时间点
    std::unique_ptr<Poller> poller_;

    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_;

    std::atomic_bool callingPendingFunctors_; //标识当前loop是否需要执行的回调操作
    std::vector<Functor> pendingFunctors_;  //存储loop需要执行的所有回调操作
    std::mutex mutex_;  //互斥锁,用来保护上面vector容器的线程安全操作
};


#endif //MY_MUDUO_EVENTLOOP_H
