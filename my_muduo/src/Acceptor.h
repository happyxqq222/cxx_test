//
// Created by Administrator on 2024-03-29.
//

#ifndef MY_MUDUO_ACCEPTOR_H
#define MY_MUDUO_ACCEPTOR_H

#include "noncopyable.h"
#include "Channel.h"
#include "Socket.h"

class EventLoop;
class InetAddress;

class Acceptor :noncopyable{
public:
    using NewConnectionCallback = std::function<void(int sockfd,const InetAddress&)>;
    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
    ~Acceptor();
    void setNewConnectionCallback(const NewConnectionCallback& cb){
        newConnectionCallback_ = std::move(cb);
    }
    bool listenning() const{
        return listenning_;
    }
    void listen();
private:
    void handleRead(Timestamp timestamp);

    EventLoop *loop_;    //Acceptor用的就是用户定义的那个baseLoop 也成为mainLoop
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
};


#endif //MY_MUDUO_ACCEPTOR_H
