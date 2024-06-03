//
// Created by Administrator on 2024/6/1.
//

#include "Session.h"

#include <iostream>
#include <boost/asio/write.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "const.h"

using namespace std;

Session::Session(boost::asio::io_context &ioc, Server *server)
    : socket_(ioc), server_(server) {
    const boost::uuids::uuid id = boost::uuids::random_generator()();
    id_ = boost::uuids::to_string(id);
}

boost::asio::ip::tcp::socket &Session::getSocket() {
    return socket_;
}

std::string &Session::getId() {
    return id_;
}

void Session::start() {

    socket_.async_read_some(
        boost::asio::buffer(data_.data(), data_.size()),
        std::bind(&Session::handleRead, this, std::placeholders::_1, std::placeholders::_2, SharedSelf()));
}

void Session::send(char *msg, int size) {
    bool pending = false;
    std::lock_guard<tbb::spin_mutex> lock(sendLock_);
    if (!sendQueue_.empty()) {
        pending = true;
    }
    std::shared_ptr<ByteBuffer> bufPtr = std::make_shared<ByteBuffer>();
    bufPtr->append(msg,size);
    sendQueue_.push(bufPtr);
    if (pending) {
        return;
    }
    auto &sendBuf = sendQueue_.front();
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(sendBuf->peek(), sendBuf->readableBytes()),
        std::bind(&Session::handleWrite, this, std::placeholders::_1, std::placeholders::_2, SharedSelf())
    );
}

void Session::close() {
    socket_.close();
    bClose_ = true;
}

std::shared_ptr<Session> Session::SharedSelf() {
    return shared_from_this();
}

Session::~Session() {
    cout << "~Session" << endl;
}


void Session::handleRead(boost::system::error_code error, size_t readedBytes,
                         std::shared_ptr<Session> sharedSelf) {
    if(!error) {
        readMsg(readedBytes);
        memset(data_.data(),0,data_.size());
        socket_.async_read_some(
            boost::asio::buffer(data_.data(),data_.size()),
            std::bind(&Session::handleRead,this,std::placeholders::_1,std::placeholders::_2,sharedSelf));
    } else {
        this->close();
        server_->clearSession(id_);
    }
}

void Session::readMsg(size_t readedBytes) {
    recvBuffer_.append(data_.data(),readedBytes);
    for(;;) {
        if(recvBuffer_.readableBytes() > HEAD_LENGTH) {
            int bodyLen  = recvBuffer_.peekInt32();
            //消息体超过最大长度，断掉连接
            if(bodyLen > MAX_CONTENT_LEN) {
                this->close();
                server_->clearSession(id_);
                break;
            }

            if(recvBuffer_.readableBytes() - HEAD_LENGTH >= bodyLen) {
                //取回头部
                recvBuffer_.retrieveInt32();
                //拿到消息体
                string msg = recvBuffer_.retrieveAsString(bodyLen);
                cout << "1 receive  size: " << bodyLen << ", data:" << msg << endl;
            } else {
                //消息体还没读完，退出循环继续注册异步读回调
                break;
            }
        } else {
            //可读字节不足一个头部，退出循环继续注册异步读回调
            break;
        }
    }
    cout << "--------------------------" << endl;
}

void Session::handleWrite(boost::system::error_code error, size_t writedBytes, std::shared_ptr<Session> sharedSelf) {
    if (!error) {
        std::lock_guard<tbb::spin_mutex> lock(sendLock_);
        cout << "send data" << sendQueue_.front()->peek() + HEAD_LENGTH << endl;
        sendQueue_.pop();
        if (!sendQueue_.empty()) {
            auto &sendBuf = sendQueue_.front();
            boost::asio::async_write(
                socket_,
                boost::asio::buffer(sendBuf->peek(), sendBuf->readableBytes()),
                bind(&Session::handleWrite, this, std::placeholders::_1, std::placeholders::_2, sharedSelf));
        } else {
            cout << "handle write failed error is " << error.what()  << endl;
            this->close();
            server_->clearSession(id_);
        }
    }
}
