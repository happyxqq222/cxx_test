//
// Created by Administrator on 2024-03-25.
//

#include "EpollPoller.h"
#include "Logger.h"
#include "Channel.h"

//channel未添加到poller中
const int kNew = -1; //channel的成员index = -1
const int kAdded = 1; //channel已添加到poller
const int kDeleted = 2; //channel从poller删除


EpollPoller::EpollPoller(EventLoop *loop)
        : Poller(loop),
          epollfd_(epoll_create1(EPOLL_CLOEXEC)),
          events_(kInitEventListSize){
    if(epollfd_ < 0){
        Logger::getLogger()->error("epoll_create error:{}",errno);
        exit(-1);
    }
}

//channel update remove => EventLoop updateChannel removeChannel => Poller(EpollPoller) updatechannel removeChannel
void EpollPoller::updateChannel(Channel *pChannel) {
    const int index = pChannel->index();
    Logger::getLogger()->info("fd={},events={},index={}",pChannel->fd(),pChannel->events(),index);

    if(index == kNew || index == kDeleted){
        int fd = pChannel->fd();
        channels_[fd] = pChannel;
        pChannel->set_index(kAdded);
        update(EPOLL_CTL_ADD,pChannel);
    }else{
        if(pChannel->isNoneEvent()){
            update(EPOLL_CTL_DEL,pChannel);
            pChannel->set_index(kDeleted);
        }else{
            update(EPOLL_CTL_MOD,pChannel);
        }
    }
}

void EpollPoller::removeChannel(Channel *channel) {
    int fd = channel->fd();
    channels_.erase(fd);

    int index = channel->index();
    if(index == kAdded){
        update(EPOLL_CTL_DEL,channel);
    }
    channel->set_index(kNew);
}

Timestamp EpollPoller::poll(int timeoutMs, Poller::ChannelList *activeChannels) {
    Logger::getLogger()->info("fd totla count:{}",channels_.size());
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(),static_cast<int>(events_.size()),timeoutMs);
    int saveErrno = errno;
    Timestamp now;
    if(numEvents > 0){
        Logger::getLogger()->debug("{} events happened",numEvents);
        fillActiveChannels(numEvents,activeChannels);
        if(numEvents == events_.size()){
            events_.resize(events_.size()*2);
        }
    }else if(numEvents == 0){
        //timeout no event happened
        Logger::getLogger()->debug("{} timeout",__FUNCTION__);
    }else{
        if(saveErrno != EINTR) {
            errno = saveErrno;
            Logger::getLogger()->error("EPoller:poll()");
        }
    }
    return Timestamp();
}

EpollPoller::~EpollPoller() {
    close(epollfd_);
}

void EpollPoller::fillActiveChannels(int numEvents, Poller::ChannelList *activeChannels) const {
    for (int i = 0; i < numEvents; i++) {
        Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

//更新channel通道 epoll_ctl
void EpollPoller::update(int operation, Channel *channel) {
    epoll_event event{};
    int fd = channel->fd();
    event.events = channel->events();
    event.data.ptr = channel;
    event.data.fd = fd;
    if(::epoll_ctl(epollfd_,operation,fd,&event) < 0){
        if(operation == EPOLL_CTL_DEL){
            Logger::getLogger()->warn("epoll_ctl del error:{},fd:{}",errno,fd);
        }else{
            Logger::getLogger()->error("epoll_ctl add/mod error:{}",errno);
        }
    }
}
