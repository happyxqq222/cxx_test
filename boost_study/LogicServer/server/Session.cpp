//
// Created by Administrator on 2024/6/3.
//

#include "Session.h"

#include <iostream>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

#include "LogicSystem.h"

using namespace std;

Session::Session(boost::asio::io_context &ioc, Server *server)
    : socket_(ioc),
      server_(server),
      data_(1024 * 2, 0) {
    boost::uuids::uuid uuidG = boost::uuids::random_generator()();
    id_ = boost::uuids::to_string(uuidG);
}

Session::~Session() {
    std::cout << "~Session destrct " << this << endl;
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
        bind(&Session::handleRead, this, placeholders::_1, placeholders::_2, sharedSelf()));
}

void Session::send(const std::shared_ptr<ByteBuffer> &buf, short msgId) {
    std::lock_guard<tbb::spin_mutex> lock(sendLock_);
    size_t sendQueueSize = sendQueue_.size();
    if (sendQueueSize >= sendQueueCapacity) {
        return;
    }
    sendQueue_.emplace(make_shared<SendNode>(buf, msgId));
    if (sendQueueSize > 0) {
        return;
    }
    auto &msgNode = sendQueue_.front();
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(msgNode->getBuf()->peek(), msgNode->getBuf()->readableBytes()),
        bind(&Session::handleWrite, this, std::placeholders::_1, std::placeholders::_2, sharedSelf())
    );
}

void Session::send(const std::string &msg, short msgId) {
    shared_ptr<ByteBuffer> buf = make_shared<ByteBuffer>();
    buf->appendInt16(msgId);
    buf->appendInt16(msg.size());
    buf->append(msg.data(),msg.size());
    send(buf,msgId);
}

void Session::close() {
    socket_.close();
}

std::shared_ptr<Session> Session::sharedSelf() {
    return shared_from_this();
}

void Session::handleRead(boost::system::error_code error, size_t readedBytes, std::shared_ptr<Session> sharedSelf) {
    try {
        if(!error) {
            recvBuf_.append(data_.data(),readedBytes);
            for (;;) {
                if(recvBuf_.readableBytes() > 4) {
                    recvBuf_.markReaderIndex();
                    short msgId = recvBuf_.readInt16();
                    short bodyLen = recvBuf_.readInt16();
                    if(recvBuf_.readableBytes() >= bodyLen) {
                        string body = recvBuf_.retrieveAsString(bodyLen);
                        shared_ptr<LogicNode> logicNode = make_shared<LogicNode>(sharedSelf,msgId,body);
                        LogicSystem::getInstance()->postMsgToQueue(logicNode);
                    } else {
                        //回退读索引
                        recvBuf_.resetReaderIndex();
                        //消息体还不全，跳出循环
                        break;
                    }
                } else {
                    //头部数据没读全，跳出循环
                    break;
                }
            }
            socket_.async_read_some(
                boost::asio::buffer(data_.data(),data_.size()),
                bind(&Session::handleRead,this,placeholders::_1,placeholders::_2,sharedSelf)
                );
        } else {
            this->close();
            server_->clearSession(id_);
        }

    } catch (exception& e) {
        cout << "Exception code is " << e.what() << endl;
    }
}

void Session::handleWrite(boost::system::error_code error, size_t writedBytes, std::shared_ptr<Session> sharedSelf) {
    try {
        if (!error) {
            std::lock_guard<tbb::spin_mutex> lock(sendLock_);
            sendQueue_.pop();
            if (!sendQueue_.empty()) {
                auto &msgNode = sendQueue_.front();
                std::shared_ptr<ByteBuffer> &buf = msgNode->getBuf();
                boost::asio::async_write(socket_,
                                         boost::asio::buffer(buf->peek(), buf->readableBytes()),
                                         bind(&Session::handleWrite, this, placeholders::_1, placeholders::_2,
                                              sharedSelf));
            }
        } else {
            std::cout << "handle write failed, error is " << error.what() << endl;
            this->close();
            server_->clearSession(id_);
        }
    } catch (exception &e) {
        std::cerr << "exception code :" << e.what() << endl;
    }
}
