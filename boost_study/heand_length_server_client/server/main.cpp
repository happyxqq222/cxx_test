#include <iostream>
#include <thread>
#include <boost/asio/io_context.hpp>

#include "BoundedBlockingQueue.h"
#include "Server.h"

using namespace std;


int main() {
    /*try {
        boost::asio::io_context ioc;
        Server server(ioc,10086);
        ioc.run();
    } catch (std::exception& e) {
        std::cerr << "exception : "  << e.what() << endl;
    }*/
    xqq::BoundedBlockingQueue<string> blockingQueue(128);
    std::thread t([&]() {
        for (int i = 0; i < 100; i++) {
            blockingQueue.push(to_string(i));
            this_thread::sleep_for(1s);
        }
    });
    std::thread t2([&]() {
        for (;;) {
            string result;
            blockingQueue.take(result);
            cout << "线程2 取到 :" << result << endl;
        }
    });

    std::thread t3([&]() {
        for (;;) {
            string result;
            blockingQueue.take(result);
            cout << "线程3 取到 :" << result << endl;
        }
    });

    getchar();
    return 0;
}
