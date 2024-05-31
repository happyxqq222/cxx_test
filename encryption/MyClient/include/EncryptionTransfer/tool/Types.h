
#pragma once

#include <muduo/net/Callbacks.h>
#include <fstream>
#include <memory>


using TupleMsg = std::tuple<muduo::net::TcpConnectionPtr,muduo::net::Buffer*,muduo::Timestamp>;
using UserConnMap = std::unordered_map<int, muduo::net::TcpConnectionPtr>;
using ConnUserMap = std::unordered_map<muduo::net::TcpConnectionPtr, int>;
using ConnTimeMap = std::unordered_map<muduo::net::TcpConnectionPtr, time_t>;

template<>
struct std::default_delete<std::ifstream>{
    void operator()(ifstream* s) const{
        s->close();
    }
};
