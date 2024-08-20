#pragma once
//框架提供的专门服务发布rpc服务的网络对象类

#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <memory>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/base/BoundedBlockingQueue.h>
#include <google/protobuf/descriptor.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <cstdlib>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include "RpcHeader.pb.h"

class RpcProvider {
    struct RpcInfo;
    using RpcHeaderPtr = std::shared_ptr<mprpc::RpcHeader>;
    using RpcHeaderVectorPtr = std::shared_ptr<std::vector<RpcHeaderPtr>>;
    using ServicePtr = std::shared_ptr<google::protobuf::Service>;
    using MethodDescriptorMap =
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*>;
    using RpcHeaderMap = std::unordered_map<muduo::net::TcpConnectionPtr, RpcHeaderVectorPtr>;
    using RpcHeaderMapPtr =  std::shared_ptr<std::unordered_map<muduo::net::TcpConnectionPtr, RpcHeaderVectorPtr>>;
public:
    RpcProvider() : rpcHeaderQueue_(4096) {}

    //这里是给框架提供给外部使用的，可以发布rpc方法的函数接口
    void registerService(const std::shared_ptr<google::protobuf::Service>& service);
    //启动rpc服务节点，开始提供rpc远程调用
    void run();
    void onConnection(const muduo::net::TcpConnectionPtr&);
    void onMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);
    void handleRpcHandler(const std::vector<std::shared_ptr<RpcInfo>>& rpcHeaders);
    void logicRun();
    //clouser的回调操作，用于序列化rpc的响应和网络发送
    void sendRpcResponse(const muduo::net::TcpConnectionPtr connptr,google::protobuf::Message* responseMsg);

private:
    //service服务类型信息
    struct ServiceInfo {
        std::shared_ptr<google::protobuf::Service> service_; //保存服务对象
        MethodDescriptorMap methodMap_; //保存服务方法
    };
    struct RpcInfo {
        RpcInfo() =default;
        muduo::net::TcpConnectionPtr connPtr_;
        RpcHeaderPtr rpcHeader_;
        std::string argStr_;
    };
    //存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string, std::shared_ptr<ServiceInfo>> serviceMap_;
    //组合EventLoop
    muduo::net::EventLoop eventLoop_;
    muduo::net::Buffer recvBuf_;
    std::thread logicthread_;
    // muduo::BoundedBlockingQueue<RpcHeaderMapPtr> rpcHeaderQueue_;
    muduo::BoundedBlockingQueue<std::vector<std::shared_ptr<RpcInfo>>> rpcHeaderQueue_;
};