//
// Created by Administrator on 2024-03-29.
//
#include "InetAddress.h"
#include "Socket.h"
#include "Logger.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <sys/types.h>

Socket::~Socket() {
    close(sockfd_);
}

void Socket::bindAddress(const InetAddress &localaddr) {
    int ret = bind(sockfd_,reinterpret_cast<const sockaddr*>(localaddr.getSockAddr()),sizeof(sockaddr_in));
    if(ret != 0){
        Logger::getLogger()->critical("bind sockfd error errno:{}",errno);
        exit(-1);
    }
}

void Socket::listen() {
    if(0 != ::listen(sockfd_,1024)){
        Logger::getLogger()->critical("listenen error errno:{}",errno);
        exit(-1);
    }
}

int Socket::accept(InetAddress *peeraddr) {
    sockaddr_in addr;
    socklen_t  len;
    bzero(&addr,sizeof(addr));
    int connfd = ::accept(sockfd_,reinterpret_cast<sockaddr*>(&addr),&len);
    if(connfd > 0 ){
        peeraddr->setSockAddr(addr);
    }
    return connfd;
}

void Socket::shutdownWrite() {
    if(::shutdown(sockfd_,SHUT_WR) < 0){
        Logger::getLogger()->info("shutdownWrite error");
    }
}

void Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 :0;
    ::setsockopt(sockfd_,IPPROTO_TCP,TCP_NODELAY,&optval,sizeof optval);
}

void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 :0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof optval);
}

void Socket::setReusePort(bool on) {
    int optval = on ? 1 :0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEPORT,&optval,sizeof optval);
}

void Socket::setKeepAlive(bool on) {
    int optval = on ? 1 :0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof optval);
}
