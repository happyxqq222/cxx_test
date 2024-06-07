//
// Created by Administrator on 2024/6/3.
//

#include "Server.h"
#include "Session.h"

#include <iostream>

using namespace boost::asio::ip;
using namespace std;

Server::Server(boost::asio::io_context &ioc, short port)
    : ioc_(ioc),
      port_(port),
      acceptor_(ioc, tcp::endpoint(tcp::v4(),port)) {
    startAccept();
}

void Server::clearSession(const std::string& id) {
    sessions_.erase(id);
}

void Server::handleAccept(std::shared_ptr<Session> newSession, const boost::system::error_code &error) {
    if(!error) {
        newSession->start();
        // sessions_.insert(newSession->getId(),newSession);
        SessionMap::accessor acc;
        sessions_.insert(std::make_pair(newSession->getId(),newSession));
    } else {
        std::cout << "session accept failed , error is " << error.what() << endl;
    }
    startAccept();
}

void Server::startAccept() {
    std::shared_ptr<Session> newSession = std::make_shared<Session>(ioc_,this);
    acceptor_.async_accept(
        newSession->getSocket(),
        bind(&Server::handleAccept,this, newSession, std::placeholders::_1)
        );
}
