//
// Created by Administrator on 2024-03-24.
//

#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <memory>

#include "Logger.h"
#include "Poller.h"
#include "EventLoop.h"
#include "Channel.h"

using namespace std;

__thread EventLoop *t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000;

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
            channel->handleEvent(pollReturnTime_);
        }
        doPendingFunctors();
    }
}

/**
 *
 */
void EventLoop::quit() {
    quit_ = true;
    if(!isInLoopThread()){
        wakeup();
    }
}

void EventLoop::runInLoop(EventLoop::Functor cb) {
    if(isInLoopThread()){
        cb();
    }else{
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(EventLoop::Functor cb) {
    {
        std::unique_lock<mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }
    if(!isInLoopThread() || callingPendingFunctors_){
        wakeup();
    }
}

void EventLoop::wakeup() {
    int64_t  data = 1;
    ssize_t n = write(wakeupFd_,&data,sizeof(data));
    if(n != sizeof data){
        Logger::getLogger()->critical("write wakeupFd error");
        exit(-1);
    }
}

void EventLoop::updateChannel(Channel *channel) {
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel) {
    return poller_->hasChannel(channel);
}

void EventLoop::handleRead(Timestamp timestamp) {
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof one);
    if(n != sizeof one){
        Logger::getLogger()->info("EventLoop::handleRead() reads {} bytes",n);
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_.store(true);
    {
        unique_lock<mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for(const auto& functor : functors){
        functor();
    }
    callingPendingFunctors_.store(false);
}
