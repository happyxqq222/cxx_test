//
// Created by Administrator on 2024/6/3.
//

#include "AsioThreadPool.h"


AsioThreadPool::AsioThreadPool(int threadNum) : work_(std::make_unique<boost::asio::io_context::work>(ioc_)){
    for (int i = 0; i < threadNum; ++i) {
        threads_.emplace_back([this]() {
           this->ioc_.run();
        });
    }
}

boost::asio::io_context &AsioThreadPool::getIoService() {
    return ioc_;
}

void AsioThreadPool::stop() {
    work_.reset();
    for(auto& t : threads_) {
        t.join();
    }
}
