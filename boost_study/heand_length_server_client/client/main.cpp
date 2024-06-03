//
// Created by Administrator on 2024/6/2.
//
#include <iostream>
#include <random>
#include <boost/asio/io_context.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "ByteBuffer.h"
#include "../server/BoundedBlockingQueue.h"


using namespace boost::asio::ip;
using namespace std;

const int MAX_LENGTH = 1024 * 2;
const int HEAD_LENGTH = 2;

using namespace std;

string randomstr(int len) {
    string str(len,0);
    // 创建一个随机数生成器
    std::random_device rd; // 用于种子
    std::mt19937 gen(rd()); // 选择梅森旋转算法作为随机数生成器
    std::uniform_int_distribution<> dis(0,25); // 定义一个范围为1到100的均匀分布
    for(int i = 0; i < len; i++) {
        str[i] = 'a' + dis(gen);
    }
    return str;
}

int main() {
    try {
        //创建上下文服务
        boost::asio::io_context ioc;
        tcp::endpoint remoteEp(address::from_string("192.168.17.128"), 10086);
        tcp::socket sock(ioc);
        boost::system::error_code error = boost::asio::error::host_not_found;
        sock.connect(remoteEp, error);
        if (error) {
            cout << "connect faield, code is " << error.value() << " error msg is " << error.message();
        }
        ByteBuffer buf;
        for (int i = 10; i < 3000; i++) {
            buf.appendInt32(i);
            string body = randomstr(i);
            buf.append(body.data(),i);
        }
        boost::asio::write(
            sock,
            boost::asio::buffer(buf.peek(),buf.readableBytes())
            );

    } catch (std::exception &e) {
        std::cerr << "excpetion: " << e.what() << endl;
    }
    return 0;
}
