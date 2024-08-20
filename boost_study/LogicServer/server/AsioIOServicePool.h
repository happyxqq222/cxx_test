//
// Created by Administrator on 2024/6/3.
//

#ifndef ASIOIOSERVICEPOOL_H
#define ASIOIOSERVICEPOOL_H
#include <thread>
#include <vector>
#include <boost/asio.hpp>

#include "common/Singleton.h"


class AsioIOServicePool : public Singleton<AsioIOServicePool>{

    friend Singleton<AsioIOServicePool>;
public:
    using IOService = boost::asio::io_context;
    using Work = boost::asio::io_context::work;
    using WorkPtr = std::unique_ptr<Work>;
    ~AsioIOServicePool() override;
    AsioIOServicePool(const AsioIOServicePool&) = delete;
    AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;
    //使用round-robin的方式返回一个io_context
    boost::asio::io_context& getIOService();
    void stop();
private:
    explicit AsioIOServicePool(std::size_t size = 2);
    std::vector<IOService> ioService_;
    std::vector<WorkPtr> works_;
    std::vector<std::thread> threads_;
    std::size_t nextIoService_;
};



#endif //ASIOIOSERVICEPOOL_H
