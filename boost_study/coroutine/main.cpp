#include <iostream>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio::ip;
using namespace boost::asio;

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;


awaitable<void> echo(tcp::socket sock) {
    try {
        char data[1024];
        for(;;) {
            std::size_t n = co_await sock.async_read_some(buffer(data),use_awaitable);
            co_await async_write(socket,buffer(data,n),use_awaitable);
        }
    }catch (std::exception& e) {
        cout << "echo:" << e.what() << endl;
    }
}

awaitable<void> listener() {
    //co_await  异步查询调度器
    auto executor = co_await this_coro::executor;
    tcp::acceptor acceptor(executor,{tcp::v4(), 10086});
    for(;;) {
        tcp::socket sock = co_await acceptor.async_accept(use_awaitable);
        co_spawn(executor,echo(std::move(sock)),detached);


    }
}

int main1(){

    try {
        io_context ioc(1);
        signal_set signals(ioc,SIGINT,SIGTERM);
        signals.async_wait([&ioc](auto,auto) {
            ioc.stop();
        });
        co_spawn(ioc,listener(),detached);
        co_spawn(ioc,listener(),detached);
        ioc.run();

    }catch (std::exception& e) {
        std::cout << "exception:" << e.what() << std::endl;
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
