//
// Created by Administrator on 2024/6/1.
//

#include "Server.h"

#include <iostream>

#include "const.h"
#include "Session.h"

using namespace std;

Server::Server(boost::asio::io_context &ioc, short port)
    : ioc_(ioc),
      port_(port),
      acceptor_(ioc, tcp::endpoint(tcp::v4(), port)) {
    static_assert(HEAD_LENGTH == 4);
    startAccept();
}

void Server::clearSession(const std::string &id) {
    size_t delCount = sessions_.erase(id);
    cout << "delCount :"  << delCount << endl;
}

void Server::handleAccept(std::shared_ptr<Session> newSession, const boost::system::error_code &error) {
    if(!error) {
        sessions_.insert(std::make_pair(newSession->getId(),newSession));
        newSession->start();
    } else {
        std::cout << "session accept failed , error is " << error.what() << std::endl;
    }
    startAccept();
}

void Server::startAccept() {
    shared_ptr<Session> newSession = make_shared<Session>(ioc_,this);
    acceptor_.async_accept(newSession->getSocket(), std::bind(&Server::handleAccept,this,newSession,placeholders::_1));
}
