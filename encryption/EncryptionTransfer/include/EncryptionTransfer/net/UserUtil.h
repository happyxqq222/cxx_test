#pragma once
#include "EncryptionTransfer/tool/AESUtils.h"
#include "EncryptionTransfer/tool/Types.h"
#include "EncryptionTransfer/handler/MessageDecode.h"

#include <muduo/net/Callbacks.h>
#include <muduo/net/TcpConnection.h>
#include <oneapi/tbb/concurrent_hash_map.h>
#include <oneapi/tbb/concurrent_unordered_map.h>
#include <oneapi/tbb/spin_mutex.h>
#include <tbb/concurrent_hash_map.h>
#include <boost/core/noncopyable.hpp>
#include <memory>
#include <optional>
#include <mutex>


using ConnCodecMap = std::unordered_map<muduo::net::TcpConnectionPtr,std::shared_ptr<MessageDecode>>;

class UserUtil : boost::noncopyable{

public:
    static std::shared_ptr<UserUtil> getInstance() {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&](){
            instance_ = std::shared_ptr<UserUtil>(new UserUtil);
        });
        return instance_;
    }

    void addUser(int id, const muduo::net::TcpConnectionPtr& conn);
    void removeUserById(int id);
    void removeUserByConnPtr(const muduo::net::TcpConnectionPtr& conn);
    std::optional<muduo::net::TcpConnectionPtr> getUserById(int id);
    std::optional<int> getUserByConnection(const muduo::net::TcpConnectionPtr& conn);
    void addOrUpdateConnTime(const muduo::net::TcpConnectionPtr& connPtr, time_t time);
    void removeConnTime(const muduo::net::TcpConnectionPtr& connPtr);
    time_t getTimeByConnPtr(const muduo::net::TcpConnectionPtr& connPtr);
    ConnTimeMap getConnTimeMap();
    MessageDecode* getMessageCodec(const muduo::net::TcpConnectionPtr& conn);
    void removeMessageCodec(const muduo::net::TcpConnectionPtr& conn);
    void addAes(const muduo::net::TcpConnectionPtr& conn,std::shared_ptr<AESUtils>& aes);
    std::shared_ptr<AESUtils> getAes(const muduo::net::TcpConnectionPtr& conn);
    ~UserUtil() = default;

private:
    UserUtil() {}
    UserConnMap userConnMap_;
    ConnUserMap connUserMap_;
    ConnTimeMap connTimeMap_;
    ConnCodecMap connCodecMap_;
    ConnAesMap connAesMap_;
    tbb::spin_rw_mutex mtx_;
    tbb::spin_rw_mutex timeMtx_;
    tbb::spin_mutex connCodecMtx_;
    static std::shared_ptr<UserUtil> instance_;
};

using UserUtilPtr = std::shared_ptr<UserUtil>;