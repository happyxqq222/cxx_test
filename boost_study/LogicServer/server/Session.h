//
// Created by Administrator on 2024/6/3.
//

#ifndef SESSION_H
#define SESSION_H
#include <queue>
#include <boost/asio/ip/tcp.hpp>
#include <boost/core/noncopyable.hpp>
#include <tbb/spin_mutex.h>

#include "SendNode.h"
#include "Server.h"


class Session : public std::enable_shared_from_this<Session>,public boost::noncopyable{

public:
    Session(boost::asio::io_context& ioc,Server* server);
    ~Session();
    boost::asio::ip::tcp::socket& getSocket();
    std::string& getId();
    void start();
    void send(const std::shared_ptr<ByteBuffer>& buf,short msgId);
    void send(const std::string& msg, short msgId);
    void close();
    std::shared_ptr<Session> sharedSelf();
private:
    void handleRead(boost::system::error_code error, size_t readedBytes,std::shared_ptr<Session> sharedSelf);
    void handleWrite(boost::system::error_code error, size_t writedBytes, std::shared_ptr<Session> sharedSelf);
    boost::asio::ip::tcp::socket socket_;
    std::string id_;
    Server *server_;
    std::queue<std::shared_ptr<SendNode>> sendQueue_;
    ByteBuffer recvBuf_;
    std::vector<char> data_;
    tbb::spin_mutex sendLock_;
    constexpr static int sendQueueCapacity = 1024;
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
};



#endif //SESSION_H
