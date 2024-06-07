#include <iostream>

#include "MqttAcl.h"
#include "MqttConfig.h"
#include "MqttLogger.h"
#include "spdlog/spdlog.h"

int main() {
    MqttLogger::getInstance()->init(MqttConfig::getInstance()->name(),
                                    MqttConfig::getInstance()->max_rotate_size(),
                                    MqttConfig::getInstance()->max_rotate_count());
    SPDLOG_INFO("test ");
    MqttAcl mqttAcl;
    mqttAcl.loadAcl("acl_rules.yml");
    return 0;
}