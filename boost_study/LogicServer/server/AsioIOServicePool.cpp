//
// Created by Administrator on 2024/6/3.
//

#include "AsioIOServicePool.h"

#include <memory>

using namespace std;

AsioIOServicePool::AsioIOServicePool(std::size_t size) : works_(size), ioService_(size), nextIoService_(0) {
    for (size_t i = 0; i < size; i++) {
        works_[i] = std::make_unique<Work>(ioService_[i]);
    }
    //遍历多个ioservice,创建使用多个线程，每个线程启动ioservice
    for (auto & ioc : ioService_) {
        threads_.emplace_back([this,&ioc]() {
           ioc.run();
        });
    }

}

AsioIOServicePool::~AsioIOServicePool() {
    std::cout << "AsioIOServicePool construct " << endl;
}

boost::asio::io_context &AsioIOServicePool::getIOService() {
    auto& service = ioService_[nextIoService_++];
    if(nextIoService_ == ioService_.size()) {
        nextIoService_ = 0;
    }
    return service;
}

void AsioIOServicePool::stop() {
    for(auto& work : works_) {
        work.reset();
    }

    for(auto& t : threads_) {
        t.join();
    }
}
