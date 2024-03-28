//
// Created by Administrator on 2024-03-28.
//

#include "EventLoopThread.h"
#include "EventLoop.h"


using  namespace std;


EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if(loop_ != nullptr){
        loop_->quit();
        thread_.join();
    }
}

EventLoop *EventLoopThread::startLoop() {
    thread_.start();  //
    EventLoop *loop = nullptr;
    {
        unique_lock<std::mutex> lock(mutex_);
        while(!loop_){
            cond_.wait(lock);
        }
        loop = loop_;
    }
    return loop;
}

//
void EventLoopThread::threadFunc() {
    EventLoop loop; //
    if(callback_){
        callback_(&loop);
    }

    {
        unique_lock<std::mutex> lock(mutex_);
        loop_ =  &loop;
        cond_.notify_one();
    }

    loop.loop();
}


