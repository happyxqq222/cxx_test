#include "EncryptionTransfer/net/UserUtil.h"
#include <oneapi/tbb/concurrent_hash_map.h>
#include <optional>

using namespace std;

void UserUtil::addUser(int id, muduo::net::TcpConnectionPtr& conn) {
    UserConnMap::const_accessor findA;
    if (!userConnMap.find(findA, id)) {
        userConnMap.insert({id,conn});
        connUserMap.insert({conn,id});
    }
}

void UserUtil::removeUser(int id) {
    UserConnMap::const_accessor findA;
    if(userConnMap.find(findA,id)) {
        connUserMap.erase(findA->second);
        userConnMap.erase(id);
    }
}

std::optional<muduo::net::TcpConnectionPtr> UserUtil::getUserById(int id) {
    UserConnMap::const_accessor findA;
    if(userConnMap.find(findA,id)) {
        return findA->second;
    }
    return std::nullopt;
}

std::optional<int> UserUtil::getUserByConnection(muduo::net::TcpConnectionPtr& conn) {
    ConnUserMap::const_accessor findA;
    if(connUserMap.find(findA,conn)) {
        return findA->second;
    }
    return nullopt;
}