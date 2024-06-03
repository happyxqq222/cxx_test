//
// Created by Administrator on 2024/6/1.
//

#ifndef SESSION_H
#define SESSION_H
#include <memory>
#include <queue>
#include <boost/asio/io_context.hpp>
#include <tbb/concurrent_queue.h>

#include "Server.h"
#include "ByteBuffer.h"
#include "const.h"


class Session : public std::enable_shared_from_this<Session>{
    using tcp = boost::asio::ip::tcp;
public:
    Session(boost::asio::io_context& ioc,Server *server);
    tcp::socket& getSocket();
    std::string& getId();
    void start();
    void send(char* msg, int size);
    void close();
    std::shared_ptr<Session> SharedSelf();
    ~Session();


private:
    void handleRead(boost::system::error_code error, size_t readedBytes, std::shared_ptr<Session> sharedSelf);
    void handleWrite(boost::system::error_code error, size_t writedBytes, std::shared_ptr<Session> sharedSelf);
    void readMsg(size_t);
    tcp::socket socket_;
    std::string id_;
    Server *server_;
    bool bClose_ = false;
    std::queue<std::shared_ptr<ByteBuffer>> sendQueue_;
    ByteBuffer recvBuffer_;
    int contentLength_ = 0;
    std::vector<char> data_ = std::vector<char>(MAX_LENGTH,0);
    tbb::spin_mutex sendLock_;
};



#endif //SESSION_H
