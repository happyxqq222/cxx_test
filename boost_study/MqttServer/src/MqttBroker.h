#pragma  once


#include <boost/smart_ptr/shared_ptr.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include "MqttCommon.h"
#include "MqttSession.h"

class MqttBroker {
    using SessionMap = std::unordered_map<std::string, std::shared_ptr<MqttSession>>;

public:
    MqttBroker();

    bool joinOrUpdate(std::shared_ptr<MqttSession> session);

    bool leave(const std::string& sid);

    void dispatch(const mqtt_packet_t& packet);

    void dispatchWill(const mqtt_packet_t& packet, const std::string& sid);

    void addRetain(const mqtt_packet_t& packet);

    void getRetain(std::shared_ptr<MqttSession> session, std::string& topicName);

    void removeRetain(const std::string& topicName);

    ~MqttBroker() = default;

private:
    uint32_t genSidCounter_;
    std::unordered_map<std::string, mqtt_packet_t> retainMap_;   
    SessionMap sessionMap_;

};