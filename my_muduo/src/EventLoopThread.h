//
// Created by Administrator on 2024-03-28.
//

#ifndef MY_MUDUO_EVENTLOOPTHREAD_H
#define MY_MUDUO_EVENTLOOPTHREAD_H

#include "noncopyable.h"
#include "Thread.h"

#include <functional>
#include <mutex>
#include <condition_variable>
#include <string>

class EventLoop;


class EventLoopThread : noncopyable{

public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    template<typename T,
            typename = std::enable_if_t<std::is_convertible_v<T,std::string>>
            >
    EventLoopThread(const ThreadInitCallback& cb ,T&& name);

    ~EventLoopThread();

    EventLoop* startLoop();

private:

    void threadFunc();

    EventLoop *loop_;
    bool exiting_;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback callback_;
};

template<typename T, typename>
EventLoopThread::EventLoopThread(const EventLoopThread::ThreadInitCallback& cb, T&& name)
        : loop_(nullptr),
          exiting_(false),
          thread_(std::bind(&EventLoopThread::threadFunc,this),std::forward<T>(name)),
          mutex_(),
          cond_(),
          callback_(cb){
}


#endif //MY_MUDUO_EVENTLOOPTHREAD_H
