//
// Created by Administrator on 2024-03-24.
//

#include "TcpServer.h"
#include "Logger.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"


using namespace std;

EventLoop* CheckLoopNotNull(EventLoop* loop){
    if(loop == nullptr){
        Logger::getLogger()->info("mainloop is null");
    }
    return loop;
}

TcpServer::TcpServer(EventLoop *loop,
                     const InetAddress &listAddr,
                     const std::string &nameArg,
                     TcpServer::Option option)
        : loop_(CheckLoopNotNull(loop)),
          ipPort_(listAddr.toIpPort()),
          name_(nameArg),
          acceptor_(std::make_unique<Acceptor>(loop,listAddr,option == Option::kReusePort) ),
          threadPool_(std::make_shared<EventLoopThreadPool>(loop,name_)),
          connectionCallback_{},
          messageCallback_{},
          nextConnId_{1}{
    //当前有用户连接，
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection,this,std::placeholders::_1,std::placeholders::_2));

}

TcpServer::~TcpServer() {

}

void TcpServer::setThreadNum(int numThreads) {

}

void TcpServer::start() {

}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {

}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {

}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {

}
