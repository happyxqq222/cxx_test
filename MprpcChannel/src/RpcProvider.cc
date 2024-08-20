#include "RpcProvider.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/Logging.h>
#include <muduo/net/Callbacks.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "MprpcApplication.h"
#include "RpcHeader.pb.h"

void RpcProvider::registerService(const std::shared_ptr<google::protobuf::Service>& service) {
    std::shared_ptr<ServiceInfo> serviceInfo = std::make_shared<ServiceInfo>();
    const google::protobuf::ServiceDescriptor* sDescriptor = service->GetDescriptor();
    //获取服务名字
    std::string serviceName = sDescriptor->name();
    //获取服务对象service的方法的数量
    int methodCnt = sDescriptor->method_count();
    for (int i = 0; i < methodCnt; i++) {
        auto methodDescriptor = sDescriptor->method(i);
        auto& methodName = methodDescriptor->name();
        serviceInfo->methodMap_.insert({ methodName, methodDescriptor });
    }
    serviceInfo->service_ = service;
    serviceMap_.insert(std::make_pair(serviceName, serviceInfo));
}

//启动rpc服务节点，开始提供rpc远程调用
void RpcProvider::run() {
    try {
        muduo::net::InetAddress inetAddress("127.0.0.1", 10086);
        MprpcConfig& config = MprpcApplication::getConfig();
        auto ipOpt = config.getConfig("serverIp");
        auto portOpt = config.getConfig("serverPort");
        std::cout << ipOpt->get() << portOpt->get() << std::endl;
        if (ipOpt && portOpt) {
            int port = std::stoi(*portOpt);
            muduo::net::InetAddress address(ipOpt.value().get(), port);
            muduo::net::TcpServer server(&eventLoop_, address, "RpcProvider");
            //绑定连接回调和消息读写回调方法 分离了网络代码和业务代码
            server.setConnectionCallback(
                std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
            server.setMessageCallback(std::bind(&RpcProvider::onMessage,
                                                this,
                                                std::placeholders::_1,
                                                std::placeholders::_2,
                                                std::placeholders::_3));
            //设置muduo库的线程数量
            server.setThreadNum(3);
            logicthread_ = std::thread(std::bind(&RpcProvider::logicRun, this));
            std::cout << "rpc 启动 ip:" << ipOpt.value().get() << portOpt.value().get()
                      << std::endl;
            //启动网络服务
            server.start();
            eventLoop_.loop();
        } else {
            throw std::runtime_error("请检查配置");
        }

    } catch (std::exception& e) {
        LOG_ERROR << "rpc启动失败 " << e.what();
        throw e;
    }
}

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr& connPtr) {
    if (!connPtr->connected()) {
        //和rpc client的连接断开
        connPtr->shutdown();
    }
}

/**
 * @brief 
 * 在框架内部 RpcProvider和RpcConsumer协商好之间通信用protobuf数据类型
 * serviceName methodName args 定义 proto的message类型，进行数据的序列化和反序列化
 * UserServiceLoginZhang san1111
 * @param connPtr 
 * @param buf 
 * @param timestamp 
 */
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr& connPtr,
                            muduo::net::Buffer* buf,
                            muduo::Timestamp timestamp) {
    recvBuf_.append(buf->peek(), buf->readableBytes());
    buf->retrieveAll();
    std::vector<std::shared_ptr<RpcInfo>> rpcInfos;
    for (;;) {
        if (buf->readableBytes() > 4) {
            int headSize = buf->peekInt32();
            if (buf->readableBytes() >= 4 + headSize) {
                // std::string rpcHeaderStr = buf->peek();
                std::string rpcHeaderStr(buf->peek() + 4, headSize);
                RpcHeaderPtr rpcHeaderPtr = std::make_shared<mprpc::RpcHeader>();
                if (rpcHeaderPtr->ParseFromString(rpcHeaderStr)) {
                    //拿到argSize
                    uint32_t argSize = rpcHeaderPtr->argssize();
                    if (buf->readableBytes() >= 4 + headSize + argSize) {
                        buf->readInt32();
                        buf->retrieveAsString(headSize);
                        std::string argStr = buf->retrieveAsString(argSize);
                        std::shared_ptr<RpcInfo> rpcInfo = std::make_shared<RpcInfo>();
                        rpcInfo->connPtr_ = connPtr;
                        rpcInfo->argStr_ = argStr;
                        rpcInfo->rpcHeader_ = rpcHeaderPtr;
                        rpcInfos.push_back(rpcInfo);
                    } else {
                        //参数部分内容不够 退出循环
                        break;
                    }
                } else {
                    connPtr->shutdown();
                    break;
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
    if (rpcInfos.size() > 0) {
        rpcHeaderQueue_.put(std::move(rpcInfos));
    }
}

void RpcProvider::handleRpcHandler(const std::vector<std::shared_ptr<RpcInfo>>& rpcHeaders) {
    for (const auto& item : rpcHeaders) {
        RpcHeaderPtr rpcHeaderPtr = item->rpcHeader_;
        std::cout << rpcHeaderPtr->methodname() << "," << rpcHeaderPtr->servicename() << ","
                  << rpcHeaderPtr->argssize() << std::endl;

        //获取service对象和method对象
        auto serviceIt = serviceMap_.find(rpcHeaderPtr->servicename());
        if (serviceIt == serviceMap_.end()) {
            std::cout << "没找到对应的serviceName:" << rpcHeaderPtr->servicename() << std::endl;
            continue;
        }
        //拿到对应的service
        ServicePtr service = serviceIt->second->service_;
        MethodDescriptorMap methodDescriptorMap = serviceIt->second->methodMap_;

        auto methodIt = methodDescriptorMap.find(rpcHeaderPtr->methodname());
        if (methodIt == methodDescriptorMap.end()) {
            continue;
        }
        //拿到对应的方法描述
        const google::protobuf::MethodDescriptor* method = methodIt->second;

        //生成rpc方法调用的请求request和相应response参数
        google::protobuf::Message* requestMsg = service->GetRequestPrototype(method).New();
        if (!requestMsg->ParseFromString(item->argStr_)) {
            std::cout << "request parse error, content:" << item->argStr_ << std::endl;
            continue;
        }
        google::protobuf::Message* responseMsg = service->GetResponsePrototype(method).New();
        //给下面method方法调用绑定一个closure的回调函数
        google::protobuf::Closure* done =
            google::protobuf::NewCallback<RpcProvider,
                                          const muduo::net::TcpConnectionPtr,
                                          google::protobuf::Message*>(
                this, &RpcProvider::sendRpcResponse, item->connPtr_, responseMsg);

        service->CallMethod(method, nullptr, requestMsg, responseMsg, done);
    }
}

void RpcProvider::logicRun() {
    std::vector<std::shared_ptr<RpcInfo>> rpcInfos = rpcHeaderQueue_.take();
    handleRpcHandler(rpcInfos);
}

//clouser的回调操作，用于序列化rpc的响应和网络发送
void RpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr connptr,
                                  google::protobuf::Message* responseMsg) {
    std::string responseStr;
    if (responseMsg->SerializeToString(&responseStr)) {
        //序列化成功后，通过网络把rpc方法执行结果发送给rpc的调用方
        int size = responseStr.size();
        connptr->send(&size, 4);
        connptr->send(responseStr);
    } else {
        std::cout << "serialize reponse_str error !" << std::endl;
    }
    connptr->shutdown(); //模拟http短链接服务，由rpcprovider主动断开连接
}