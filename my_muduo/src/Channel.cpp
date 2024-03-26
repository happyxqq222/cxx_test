//
// Created by Administrator on 2024-03-24.
//

#include <sys/epoll.h>

#include "Channel.h"
#include "Timestamp.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Timestamp.h"

using namespace std;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
        : fd_(fd),
        loop_(loop),
        events_(0),
        revents_(0),
        index_(-1),
        tied_(false){

}

Channel::~Channel() {

}

void Channel::handleEvent(Timestamp receiveTime) {
    shared_ptr<void> guard;
    guard = tie_.lock();
    if(guard){
        handleEventWithGuard(receiveTime);
    }else{
        handleEventWithGuard(receiveTime);
    }
}

void Channel::tie(const std::shared_ptr<void> & obj) {
    tie_ = obj;
    tied_ = true;
}

//在channel所属的event中，把当前的channel删除掉
void Channel::remove() {
//    loop_->removeChannel(this);
}

/**
 * 当改变channel所表示的fd的events事件后，upate负责在Poller里面更改fd相应的事件epoll_ctl
 */
void Channel::update() {
    //通过channel所属的eventloop，调用poller的相应方法,注册fd的event事件
    //todo
//    loop_->updateChannel(this);
}

//根据poller通知channel发生的具体事件,由channel负责调用具体的回调操作
void Channel::handleEventWithGuard(Timestamp receiveTime) {
    Logger::getLogger()->info("channel handleEvent revents:{}",revents_);
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
        if(closeCallback_){
            closeCallback_();
        }
    }
    if(revents_ & EPOLLERR){
        errorCallback_();
    }
    if(revents_ & (EPOLLIN | EPOLLPRI)){
        if(readCallback_){
            readCallback_(receiveTime);
        }
    }
    if(revents_ & EPOLLOUT){
        if(writeCallback_){
            writeCallback_();
        }
    }

}
