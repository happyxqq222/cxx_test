#pragma once
#include "EncryptionTransfer/tool/Singleton.h"

#include <muduo/net/Callbacks.h>
#include <muduo/net/TcpConnection.h>
#include <oneapi/tbb/concurrent_hash_map.h>
#include <tbb/concurrent_hash_map.h>
#include <optional>


class UserUtil : public Singleton<UserUtil>{
    friend class Singleton<UserUtil>;
public:
    using UserConnMap = tbb::concurrent_hash_map<int, muduo::net::TcpConnectionPtr>;
    using ConnUserMap = tbb::concurrent_hash_map<muduo::net::TcpConnectionPtr, int>;
    UserUtil() = default;
    void addUser(int id, muduo::net::TcpConnectionPtr& conn);
    void removeUser(int id);
    std::optional<muduo::net::TcpConnectionPtr> getUserById(int id);
    std::optional<int> getUserByConnection(muduo::net::TcpConnectionPtr& conn);
    ~UserUtil() = default;

private:
    UserConnMap userConnMap;
    ConnUserMap connUserMap;
};