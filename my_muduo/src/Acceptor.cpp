//
// Created by Administrator on 2024-03-29.
//

#include "Acceptor.h"
#include "Logger.h"
#include "Timestamp.h"
#include "InetAddress.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <functional>
#include <errno.h>
#include <unistd.h>

using namespace std;

static int createNonBlockingOrDie(){
    int sockfd = ::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_TCP);
    if(sockfd < 0){
        Logger::getLogger()->critical("create socket error!");
        exit(-1);
    }
    return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport)
        : loop_(loop)
        , acceptSocket_(createNonBlockingOrDie())
        , acceptChannel_(loop,acceptSocket_.fd())
        , listenning_(false){
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(true);
    acceptSocket_.bindAddress(listenAddr);
    //如果有新用户连接 执行 回调 (connfd->channle->唤醒subloop)
    acceptChannel_.setReadCallback(bind(&Acceptor::handleRead,this,std::placeholders::_1));
}

Acceptor::~Acceptor() {
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void Acceptor::listen() {
    listenning_ = true;
    acceptSocket_.listen();   //listen
    acceptChannel_.enableReading();
}

//listenfd有新用户连接
void Acceptor::handleRead(Timestamp timestamp) {
    //有新连接
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if(connfd >= 0){
        if(newConnectionCallback_){
            newConnectionCallback_(connfd,peerAddr);
        }else{
            ::close(connfd);
        }
    }else{
        Logger::getLogger()->error("acceptor error errno:{}",errno);
        if(errno == EMFILE){
            Logger::getLogger()->error("EMFILE error");
        }
    }

}
