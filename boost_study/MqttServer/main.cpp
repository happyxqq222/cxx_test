#include <exception>
#include <iostream>

#include "MqttAcl.h"
#include "MqttConfig.h"
#include "MqttException.h"
#include "MqttLogger.h"
#include "spdlog/spdlog.h"

using namespace std;

void testStacktrace() { /* throw MqttException("error mqtt"); */ }

struct ss {
    uint8_t l1 :2;
    uint8_t l2 :1;
    uint8_t l3 :3;
};

int main() {

    cout << sizeof(ss) << endl;

    MqttLogger::getInstance()->init(MqttConfig::getInstance()->name(),
                                    MqttConfig::getInstance()->max_rotate_size(),
                                    MqttConfig::getInstance()->max_rotate_count());
    try {
        testStacktrace();
    } catch (MqttException& e) {
        SPDLOG_INFO("exception:{},trace:{}", e.what(), e.getStackTrace());
    }


    // SPDLOG_INFO("test ");
    // MqttAcl mqttAcl;
    // mqttAcl.loadAcl("acl_rules.yml");
    return 0;
}