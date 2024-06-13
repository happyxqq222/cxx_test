
#include "MqttIoServicePool.h"

#include <boost/asio/io_context.hpp>
#include <cstddef>
#include <memory>
#include <iostream>
#include "spdlog/spdlog.h"

using namespace std;
using namespace boost;
using namespace boost::asio;

MqttIoServicePool::MqttIoServicePool(std::size_t size) : iocs_(size), works_(size), nextIoc_(0) {
    for (size_t i = 0; i < size; i++) {
        works_[i] = make_unique<Work>(iocs_[i]);
    }

    for (size_t i = 0; i < size; i++) {
        threads_.emplace_back([this, i]() { iocs_[i].run(); });
    }
}

io_context& MqttIoServicePool::getIoContext() {
    auto& ioc = iocs_[nextIoc_++];
    if(nextIoc_ == iocs_.size()) {
        nextIoc_ = 0;
    }
    return ioc;
}

void MqttIoServicePool::stop() {
    for(auto& work : works_) {
        work.reset();
    }
    for(auto& t : threads_) {
        t.join();
    }

}

MqttIoServicePool& MqttIoServicePool::getInstance() {
    static MqttIoServicePool msp;
    return msp;
}

MqttIoServicePool::~MqttIoServicePool() {
    SPDLOG_INFO("~MqttIoServicePool");
}