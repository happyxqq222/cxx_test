//
// Created by Administrator on 2024-03-24.
//

#include "InetAddress.h"

using namespace std;

std::string InetAddress::toIp() const {
    char buf[64] = {0};
    ::inet_ntop(AF_INET,&addr_.sin_addr, buf, sizeof buf);
    return buf;
}

std::string InetAddress::toIpPort() const {
    char buf[64] = {0};
    ::inet_ntop(AF_INET,&addr_.sin_addr, buf, sizeof buf);
    uint16_t port = ntohs(addr_.sin_port);
    string ipPort;
    ipPort.append(buf).append(":").append(to_string(port));
    return ipPort;
}

uint16_t InetAddress::toPort() const {
    return ntohs(addr_.sin_port);
}
