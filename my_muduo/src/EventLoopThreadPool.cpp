//
// Created by Administrator on 2024-03-28.
//

#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"

#include <memory>

using namespace std;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, std::string_view nameArg)
        : baseLoop_(baseLoop),
          name_(nameArg),
          started_(false),
          numThreads_(0),
          next_(0) {

}

EventLoopThreadPool::~EventLoopThreadPool() {

}

void EventLoopThreadPool::start(const EventLoopThreadPool::ThreadInitCallback &cb) {
    started_ = true;
    for (int i = 0; i < numThreads_; i++) {
        char buf[name_.size()+32];
        snprintf(buf,sizeof(buf),"%s%d",name_.c_str(),i);
        unique_ptr<EventLoopThread> t = make_unique<EventLoopThread>(cb,static_cast<const char*>(buf));
        EventLoopThread* pLoopThread = t.get();
        threads_.push_back(std::move(t));
        loops_.push_back(pLoopThread->startLoop());
    }

    //整个服务端只有一个线程，运行着baseloop
    if(numThreads_ == 0 && cb){
        cb(baseLoop_);
    }
}

EventLoop *EventLoopThreadPool::getNextLoop() {
    EventLoop *loop = baseLoop_;
    if(!loops_.empty()){
        loop = loops_[next_];
        next_++;
        if(next_ == loops_.size()){
            next_ = 0;
        }
    }
    return loop;
}

std::vector<EventLoop *> EventLoopThreadPool::getAllLoops() {
    if(loops_.empty()){
        return std::vector<EventLoop*>{1,baseLoop_};
    }else{
        return loops_;
    }
}
