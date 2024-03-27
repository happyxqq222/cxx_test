//
// Created by Administrator on 2024-03-24.
//

#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>

#include "Logger.h"
#include "Poller.h"
#include "EventLoop.h"
#include "Channel.h"

//防止一个线程创建多个EventLoop
__thread EventLoop *t_loopInThisThread = nullptr;

//定义默认的Poller IO复用接口超时时间
const int kPollTimeMs = 10000;

//创建wakeupfd 用来notify唤醒subReactor出来新来的channel
int createEventfd(){
    int evetfd = ::eventfd(0,EFD_NONBLOCK| EFD_CLOEXEC);
    if(evetfd < 0){
        Logger::getLogger()->error("eventfd failure {}",errno);
        exit(-1);
    }
    return evetfd;
}

EventLoop::EventLoop()
        : threadId_(CurrentThread::tid()),
          quit_(false),
          looping_(false),
          callingPendingFunctors_(false),
          poller_(Poller::newDefaultPooler(this)),
          wakeupFd_(createEventfd()),
          wakeupChannel_(new Channel(this, wakeupFd_)),
          currentActiveChannel_(nullptr){

    std::stringstream thisAddr;
    thisAddr << std::hex << this;
    Logger::getLogger()->info("EventLoop created {} in thread {}",thisAddr.str(),threadId_);

    if(t_loopInThisThread){
        std::stringstream  ss;
        ss << std::hex << t_loopInThisThread;
        Logger::getLogger()->critical("Another Eventloop {} exists in this thread {}",ss.str(),threadId_);
    }else{
        t_loopInThisThread = this;
    }

    //设置wakeupfd事件类型以及发生事件后回调操作
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead,this,std::placeholders::_1));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
    looping_ = true;
    quit_ = false;
    Logger::getLogger()->info("EventLoop {} start looping ",pthread_self());

    while(!quit_){
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for(Channel* channel : activeChannels_){
            //Poller监听哪些channel发生事件，然后上报给EventLoop,通知channel发生了哪些事件
            channel->handleEvent(pollReturnTime_);
        }
        //执行当前eventloop事件循环需要处理的回调操作
        /**
         * IO线程 mainLoop accept 接收新用户连接 channel->subloop
         * mainloop 事先注册一个回调cb (需要subloop执行)
         */
        doPendingFunctors();
    }
}

void EventLoop::quit() {
    quit_ = true;
    if(!isInLoopThread()){
        wakeup();
    }
}

void EventLoop::runInLoop(EventLoop::Functor cb) {

}

void EventLoop::queueInLoop(EventLoop::Functor cb) {

}

void EventLoop::wakeup() {

}

void EventLoop::updateChannel(Channel *channel) {

}

void EventLoop::removeChannel(Channel *channel) {

}

bool EventLoop::hasChannel(Channel *channel) {
    return false;
}

void EventLoop::handleRead(Timestamp timestamp) {
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof one);
    if(n != sizeof one){
        Logger::getLogger()->info("EventLoop::handleRead() reads {} bytes",n);
    }
}

void EventLoop::doPendingFunctors() {

}
