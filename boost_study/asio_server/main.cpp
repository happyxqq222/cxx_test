#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <cstddef>
#include <exception>
#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <thread>

using namespace boost;
using namespace boost::asio::ip;
using namespace std;

constexpr int MAX_LENGTH = 1024;

typedef std::shared_ptr<tcp::socket> SocketPtr;
set<std::shared_ptr<std::thread>> threadSet;

void session(SocketPtr sock) {
    try{
        for(;;) {
            char data[1024]{};
            boost::system::error_code error;
            size_t length = sock->read_some(asio::buffer(data,MAX_LENGTH),error);
            if(error == boost::asio::error::eof) {
                std::cout << "connection closed by peer"  << endl;
                sock->close();
                break;
            }
            else if(error) {
                throw boost::system::system_error(error);
            } else {
                cout << "receive from:" << sock->remote_endpoint().address().to_string()  << endl;
                cout << "receive message is " << data << endl;
                //回传给对方
                boost::asio::write(*sock,boost::asio::buffer(data,length));
            }

        }
    }catch(std::exception& e){
        std::cout << "exception:" << e.what() << endl;
    }
}

void server(asio::io_context& ioc,unsigned short port) {
    tcp::acceptor a(ioc,tcp::endpoint(tcp::v4(),port));
    for(;;) {
        SocketPtr socket{new tcp::socket(ioc)};
        a.accept(*socket);
        auto t = std::make_shared<thread>(session,socket);
        threadSet.insert(t);
    }
}

int main() {
    try{
        boost::asio::io_context ioc;
        server(ioc,10086);
        for(auto& t : threadSet) {
            if(t->joinable()) {
                t->join();
            }
        }
    } catch(std::exception& e) {
        cout << e.what() << endl;
    }
}