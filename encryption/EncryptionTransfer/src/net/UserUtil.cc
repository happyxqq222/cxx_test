#include "EncryptionTransfer/net/UserUtil.h"
#include "EncryptionTransfer/handler/MessageDecode.h"

#include <muduo/base/Logging.h>
#include <oneapi/tbb/concurrent_hash_map.h>
#include <oneapi/tbb/concurrent_queue.h>
#include <oneapi/tbb/spin_mutex.h>
#include <oneapi/tbb/spin_rw_mutex.h>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>

using namespace std;
using namespace tbb;

std::shared_ptr<UserUtil> UserUtil::instance_ = nullptr;

void UserUtil::addUser(int id, const muduo::net::TcpConnectionPtr& conn) {
    std::unique_lock<tbb::spin_rw_mutex> lock(mtx_);
    auto it = userConnMap_.find(id);
    if (it == userConnMap_.end()) {
        userConnMap_.insert({ id, conn });
        connUserMap_.insert({ conn, id });
    }
}

void UserUtil::removeUserById(int id) {
    std::unique_lock<tbb::spin_rw_mutex> lock(mtx_);
    auto it = userConnMap_.find(id);
    if (it != userConnMap_.end()) {
        connUserMap_.erase(it->second);
        userConnMap_.erase(id);
    }
}

void UserUtil::removeUserByConnPtr(const muduo::net::TcpConnectionPtr& conn) {
    std::unique_lock<tbb::spin_rw_mutex> lock(mtx_);
    auto it = connUserMap_.find(conn);
    if(it != connUserMap_.end()) {
        userConnMap_.erase(it->second);
        connUserMap_.erase(it);
    }
}

std::optional<muduo::net::TcpConnectionPtr> UserUtil::getUserById(int id) {
    std::shared_lock<tbb::spin_rw_mutex> lock(mtx_);
    auto it = userConnMap_.find(id);
    if (it != userConnMap_.end()) {
        return it->second;
    }
    return nullopt;
}

std::optional<int> UserUtil::getUserByConnection(const muduo::net::TcpConnectionPtr& conn) {
    std::shared_lock<tbb::spin_rw_mutex> lock(mtx_);
    auto it = connUserMap_.find(conn);
    if (it != connUserMap_.end()) {
        return it->second;
    }
    return nullopt;
}

void UserUtil::addOrUpdateConnTime(const muduo::net::TcpConnectionPtr& connPtr, time_t time) {
    std::unique_lock<tbb::spin_rw_mutex> lock(timeMtx_);
    connTimeMap_[connPtr] = time;
}

void UserUtil::removeConnTime(const muduo::net::TcpConnectionPtr& connPtr) {
    std::unique_lock<tbb::spin_rw_mutex> lock(timeMtx_);
    connTimeMap_.erase(connPtr);
}

time_t UserUtil::getTimeByConnPtr(const muduo::net::TcpConnectionPtr& connPtr) {
    std::shared_lock<tbb::spin_rw_mutex> lock(timeMtx_);
    auto it = connTimeMap_.find(connPtr);
    if (it != connTimeMap_.end()) {
        return it->second;
    }
    return -1;
}

ConnTimeMap UserUtil::getConnTimeMap() {
    std::unique_lock<tbb::spin_rw_mutex> lock(timeMtx_);
    ConnTimeMap connTimeMap = connTimeMap_;
    return connTimeMap;
}


MessageDecode* UserUtil::getMessageCodec(const muduo::net::TcpConnectionPtr& conn) {
    std::unique_lock<tbb::spin_mutex> lock(connCodecMtx_);
    auto it =  connCodecMap_.find(conn);
    if(it == connCodecMap_.end()) {
        std::shared_ptr<MessageDecode> messageCodec = make_shared<MessageDecode>();
        connCodecMap_[conn]  = messageCodec;
        return messageCodec.get();
    }
    return it->second.get();
}

void UserUtil::removeMessageCodec(const muduo::net::TcpConnectionPtr& conn) {
    std::unique_lock<tbb::spin_mutex> lock(connCodecMtx_);
    connCodecMap_.erase(conn);
}

void UserUtil::addAes(const muduo::net::TcpConnectionPtr& conn,std::shared_ptr<AESUtils>& aes) {
    connAesMap_[conn] = aes;
}

std::shared_ptr<AESUtils> UserUtil::getAes(const muduo::net::TcpConnectionPtr& conn) {
    auto it = connAesMap_.find(conn);
    if(it == connAesMap_.end()) {
        return nullptr;
    }
    return it->second;
}