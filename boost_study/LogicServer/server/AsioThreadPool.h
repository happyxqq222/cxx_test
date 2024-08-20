//
// Created by Administrator on 2024/6/3.
//

#ifndef ASIOTHREADPOOL_H
#define ASIOTHREADPOOL_H

#include "common/Singleton.h"
#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>

class AsioThreadPool : public Singleton<AsioThreadPool> ,boost::noncopyable{
public:
    friend class Singleton<AsioThreadPool>;
    ~AsioThreadPool() override= default;
    boost::asio::io_context& getIoService();
    void stop();

private:
    AsioThreadPool(int threadNum = std::thread::hardware_concurrency());
    boost::asio::io_context ioc_;
    std::unique_ptr<boost::asio::io_context::work> work_;
    std::vector<std::thread> threads_;
};



#endif //ASIOTHREADPOOL_H
