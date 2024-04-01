//
// Created by Administrator on 2024-03-24.
//

#ifndef MY_MUDUO_INETADDRESS_H
#define MY_MUDUO_INETADDRESS_H
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include "Logger.h"

class InetAddress {
public:
    template<typename T,
            typename  = std::enable_if_t<std::is_convertible_v<T,std::string>,std::void_t<>>
            >
    explicit InetAddress(uint16_t port, T&& ip);
    InetAddress() = default;

    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;
    const sockaddr_in* getSockAddr() const{
        return &addr_;
    }
    void setSockAddr(const sockaddr_in &addr) {addr_ = addr;}

private:

    sockaddr_in addr_;
};

template<typename T, typename>
InetAddress::InetAddress(uint16_t port, T &&ip) {
    std::string ip_str = std::forward<T>(ip);
    bzero(&addr_,sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
//    addr_.sin_addr.s_addr =  inet_addr(ip_str.c_str());
    inet_aton(ip_str.c_str(),&addr_.sin_addr);
}

#endif //MY_MUDUO_INETADDRESS_H
