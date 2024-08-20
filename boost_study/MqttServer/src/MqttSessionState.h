#pragma once

#include <cstdint>
#include <queue>
#include <string>
#include <unordered_map>
#include "MqttCommon.h"
struct MqttSessionState {
    bool cleanSession_;
    uint16_t keepAlive_;
    uint16_t packetIdGen_;
    mqtt_packet_t willTopic;
    //存储客户端的订阅的主题
    std::unordered_map<std::string, uint8_t> subTopicMap_;
    std::unordered_map<uint16_t,mqtt_packet_t> waitingMap_;
    std::queue<mqtt_packet_t> inflightQueue_;

    MqttSessionState() :cleanSession_(true), keepAlive_(0U), packetIdGen_(0U){}

};