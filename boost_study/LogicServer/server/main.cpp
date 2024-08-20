//
// Created by Administrator on 2024/6/2.
//

#include <iostream>
#include <thread>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/thread/latch.hpp>

#include "AsioThreadPool.h"
#include "Server.h"

using namespace std;

int main() {
    boost::latch l(1);
    try {
        auto pool = AsioThreadPool::getInstance();
        boost::asio::io_context ioc;
        boost::asio::signal_set signals(ioc,SIGINT,SIGTERM);
        signals.async_wait([&ioc,&pool,&l](auto, auto) {
            cout << "收到中断信号 " << this_thread::get_id() << endl;
            pool->stop();
            ioc.stop();
            l.count_down();
        });
        Server server(pool->getIoService(),10086);
        cout << "主线程:" << this_thread::get_id() << " 启动" << endl;
        ioc.run();
        l.wait();
    } catch (std::exception &e) {
        std::cerr << "excpeiton : " << e.what() << endl;
    }
    return 0;
}
