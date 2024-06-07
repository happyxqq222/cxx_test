//
// Created by Administrator on 2024/6/2.
//

#include <iostream>
#include <thread>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

#include "Server.h"

using namespace std;

int main() {
    try {
        boost::asio::io_context ioc;
        boost::asio::signal_set signals(ioc,SIGINT,SIGTERM);
        signals.async_wait([&ioc](auto, auto) {
            cout << "收到中断信号 " << this_thread::get_id() << endl;
            ioc.stop();
        });
        Server server(ioc,10086);
        cout << "主线程:" << this_thread::get_id() << " 启动" << endl;
        ioc.run();
    } catch (std::exception &e) {
        std::cerr << "excpeiton : " << e.what() << endl;
    }
    return 0;
}
