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

//��ֹһ���̴߳������EventLoop
__thread EventLoop *t_loopInThisThread = nullptr;

//����Ĭ�ϵ�Poller IO���ýӿڳ�ʱʱ��
const int kPollTimeMs = 10000;

//����wakeupfd ����notify����subReactor����������channel
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

    //����wakeupfd�¼������Լ������¼���ص�����
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
            //Poller������Щchannel�����¼���Ȼ���ϱ���EventLoop,֪ͨchannel��������Щ�¼�
            channel->handleEvent(pollReturnTime_);
        }
        //ִ�е�ǰeventloop�¼�ѭ����Ҫ����Ļص�����
        /**
         * IO�߳� mainLoop accept �������û����� channel->subloop
         * mainloop ����ע��һ���ص�cb (��Ҫsubloopִ��)
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
