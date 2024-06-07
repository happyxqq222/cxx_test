//
// Created by Administrator on 2024/6/3.
//

#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <boost/asio.hpp>
#include <memory>
#include <tbb/concurrent_hash_map.h>

class Session;

class Server {

    using SessionMap = tbb::concurrent_hash_map<std::string,std::shared_ptr<Session>>;
public:
    Server(boost::asio::io_context& ioc, short port);
    void clearSession(const std::string&);

private:
    void handleAccept(std::shared_ptr<Session>, const boost::system::error_code& error);
    void startAccept();
    boost::asio::io_context& ioc_;
    short port_;
    boost::asio::ip::tcp::acceptor acceptor_;
    SessionMap sessions_;
};



#endif //SERVER_H
