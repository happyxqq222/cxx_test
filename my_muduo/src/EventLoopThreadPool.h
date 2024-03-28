//
// Created by Administrator on 2024-03-28.
//

#ifndef MY_MUDUO_EVENTLOOPTHREADPOOL_H
#define MY_MUDUO_EVENTLOOPTHREADPOOL_H

#include "noncopyable.h"

#include <functional>
#include <string_view>
#include <string>
#include <vector>
#include <memory>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : noncopyable{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThreadPool(EventLoop *baseLoop, std::string_view nameArg);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads){
        numThreads_ = numThreads;
    }

    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    //如果工作在多线程中，baseLoop_默认以轮询方式分配channel给subloop
    EventLoop* getNextLoop();

    std::vector<EventLoop*> getAllLoops();

    bool started() const {
        return started_;
    }

    const std::string& name() const{
        return name_;
    }

private:
    EventLoop* baseLoop_;
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};


#endif //MY_MUDUO_EVENTLOOPTHREADPOOL_H
