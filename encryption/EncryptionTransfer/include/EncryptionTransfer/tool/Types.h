
#pragma once

#include <muduo/net/Callbacks.h>
#include <fstream>
#include <memory>
#include <unordered_map>
#include "EncryptionTransfer/tool/AESUtils.h"


using TupleMsg = std::tuple<muduo::net::TcpConnectionPtr,muduo::net::Buffer*,muduo::Timestamp>;
using UserConnMap = std::unordered_map<int, muduo::net::TcpConnectionPtr>;
using ConnUserMap = std::unordered_map<muduo::net::TcpConnectionPtr, int>;
using ConnTimeMap = std::unordered_map<muduo::net::TcpConnectionPtr, time_t>;
using ConnAesMap  = std::unordered_map<muduo::net::TcpConnectionPtr, std::shared_ptr<AESUtils>>;

template<>
struct std::default_delete<std::ifstream> {
    void operator()(ifstream* ifptr)  const {
        ifptr->close();
    }
};

