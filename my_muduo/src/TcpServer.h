//
// Created by Administrator on 2024-03-24.
//

#ifndef MY_MUDUO_TCPSERVER_H
#define MY_MUDUO_TCPSERVER_H

#include "noncopyable.h"
#include "Callbacks.h"

#include <functional>
#include <string>
#include <memory>
#include <atomic>
#include <unordered_map>

class EventLoop;
class Acceptor;
class EventLoopThreadPool;
class InetAddress;

class TcpServer : noncopyable{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    enum class Option{
        kNonReusePort,
        kReusePort,
    };

    TcpServer(EventLoop *loop,
              const InetAddress &listAddr,
              const std::string &nameArg,
              Option option = Option::kNonReusePort);
    ~TcpServer();

    void setThreadInitcallback(const ThreadInitCallback& cb){
        threadInitCallback_ = cb;
    }
    void setConnectionCallback(const ConnectionCallback& cb){
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback& cb){
        messageCallback_ = cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb){
        writeCompleteCallback_ = cb;
    }
    //设置底层subloop个数
    void setThreadNum(int numThreads);
    //开启服务监听
    void start();
private:

    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    using ConnectionMap = std::unordered_map<std::string,TcpConnectionPtr>;

    EventLoop *loop_;    //baseLoop用户定义的loop
    const std::string ipPort_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;    //运行mainLoop  任何就是监听新连接事件
    std::shared_ptr<EventLoopThreadPool> threadPool_;  //one loop per thread

    ConnectionCallback connectionCallback_; //有新连接的回调
    MessageCallback messageCallback_;  //有读写消息的回调
    WriteCompleteCallback writeCompleteCallback_; //消息发送完成以后的回调

    ThreadInitCallback threadInitCallback_;  //loop线程初始化的回调
    std::atomic_int started_;

    int nextConnId_;
    ConnectionMap connections_; //保存所有的连接
};


#endif //MY_MUDUO_TCPSERVER_H
