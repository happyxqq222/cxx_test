//
// Created by Administrator on 2024/6/2.
//
#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <json/value.h>

#include "common/ByteBuffer.h"

using namespace std;
using namespace boost::asio::ip;


int main() {
    try {
        boost::asio::io_context ioc;
        tcp::endpoint remoteEp(address::from_string("192.168.17.128"),10086);
        tcp::socket sock(ioc);
        boost::system::error_code error = boost::asio::error::host_not_found;
        sock.connect(remoteEp,error);
        if(error) {
            cout << "connect failed,code is " << error.value() << "error msg is " << error.message();
            return 0;
        }
        Json::Value root;
        root["id"] = 1;
        root["data"] = "hello world";
        std::string msg = root.toStyledString();
        ByteBuffer buf;
        buf.appendInt16(1);
        buf.appendInt16(msg.size());
        buf.append(msg.data(),msg.size());
        boost::asio::write(sock,boost::asio::buffer(buf.peek(),buf.readableBytes()));
        string recvMsg(256,0);
        size_t recvLen = sock.read_some(boost::asio::buffer(recvMsg.data(),recvMsg.size()));
        cout << "收到 :" << string(recvMsg.data()+4,recvMsg.data()+recvMsg.length()) << endl;
    } catch (std::exception& e) {
        std::cerr << "exception :" << e.what() << endl;
    }
    return 0;
}