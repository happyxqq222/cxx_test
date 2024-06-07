//
// Created by Administrator on 2024/6/1.
//

#ifndef SERVER_H
#define SERVER_H
#include <map>
#include <boost/asio/error.hpp>
#include <boost/asio/ip/tcp.hpp>


class Session;

class Server {
    using tcp = boost::asio::ip::tcp;
public:
    Server(boost::asio::io_context& ioc,short port);
    void clearSession(const std::string& id);

private:
    void handleAccept(std::shared_ptr<Session> session, const boost::system::error_code &error);
    void startAccept();
    std::map<std::string,std::shared_ptr<Session>> sessions_;
    tcp::acceptor acceptor_;
    boost::asio::io_context &ioc_;
    short port_;
};



#endif //SERVER_H
