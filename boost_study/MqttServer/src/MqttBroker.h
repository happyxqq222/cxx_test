#pragma once

#include <oneapi/tbb/concurrent_hash_map.h>
#include <tbb/concurrent_hash_map.h>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

#include "MqttCommon.h"
#include "MqttSession.h"

using SessionMap = tbb::concurrent_hash_map<std::string, std::shared_ptr<MqttSession>>;
using RetainMap = tbb::concurrent_hash_map<std::string, mqtt_packet_t>;

// template <>
// struct std::default_delete<SessionMap::accessor> {
//     void operator()(SessionMap::accessor* p) const {
//         p->release();
//     }
// };

// template<>
// struct std::default_delete<SessionMap::const_accessor> {
//     void operator()(SessionMap::const_accessor* p) const {
//         p->release();
//     }
// };

struct SessionMapAccessorDeleter {
    void operator()(typename tbb::concurrent_hash_map<std::string, std::shared_ptr<MqttSession>>::accessor* p) const {
        if (p) {
            p->release();
            // delete p;
        }
    }
};

struct SessionMapConstAccessorDeleter {
    void operator()(typename tbb::concurrent_hash_map<std::string, mqtt_packet_t>::const_accessor* p) const {
        if(p){
            p->release();
            // delete p;
        }
    }
};

struct RetainMapAccessorDeleter {
    void operator()(typename tbb::concurrent_hash_map<std::string, mqtt_packet_t>::accessor* p) const {
        if (p) {
            p->release();
            // delete p;
        }
    }
};

struct RetainMapConstAccessorDeleter {
    void operator()(typename tbb::concurrent_hash_map<std::string, std::shared_ptr<MqttSession>>::const_accessor* p) const {
        if(p){
            p->release();
            // delete p;
        }
    }
};



class MqttBroker {
public:
    MqttBroker();

    bool joinOrUpdate(std::shared_ptr<MqttSession> session);

    void leave(const std::string& sid);

    void dispatch(const mqtt_packet_t& packet);

    void dispatchWill(const mqtt_packet_t& packet, const std::string& sid);

    void addRetain(const mqtt_packet_t& packet);

    void getRetain(std::shared_ptr<MqttSession> session, const std::string& subTopic);

    void removeRetain(const std::string& topicName);

    ~MqttBroker() = default;

private:
    uint32_t genSidCounter_;
    RetainMap retainMap_;
    SessionMap sessionMap_;
};