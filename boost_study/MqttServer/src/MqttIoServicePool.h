#pragma once

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/core/noncopyable.hpp>
#include <cstddef>
#include <memory>
#include <thread>
#include <vector>

/**
 * @brief 
 * 在不同的线程中启动io_context
 */
class MqttIoServicePool : public boost::noncopyable {
public:
    using Io_context = boost::asio::io_context;
    using Work = boost::asio::io_context::work;
    using WorkPtr = std::unique_ptr<Work>;

    Io_context& getIoContext();

    void stop();

    static MqttIoServicePool& getInstance();

    ~MqttIoServicePool();
private:
    MqttIoServicePool(std::size_t size = std::thread::hardware_concurrency());
    std::vector<Io_context> iocs_;
    std::vector<WorkPtr> works_;
    std::vector<std::thread> threads_;
    std::size_t nextIoc_;
};