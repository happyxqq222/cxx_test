//
// Created by xqq on 24-6-6.
//

#ifndef MQTTSERVER_MQTTLOGGER_H
#define MQTTSERVER_MQTTLOGGER_H

#include "Singleton.h"
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class MqttLogger : public Singleton<MqttLogger>{
public:
    friend Singleton<MqttLogger> ;

    bool init(const std::string& logFile, uint64_t maxRotateSize, uint64_t maxRotateCount);

    MqttLogger(const MqttLogger&) = delete;
    MqttLogger& operator=(const MqttLogger&) = delete;
    MqttLogger(MqttLogger&&) = delete;
    MqttLogger& operator=(MqttLogger&&) = delete;
private:
   MqttLogger() = default;

};


#endif //MQTTSERVER_MQTTLOGGER_H
