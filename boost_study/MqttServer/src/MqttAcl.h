//
// Created by xqq on 24-6-6.
//

#ifndef MQTTSERVER_MQTTACL_H
#define MQTTSERVER_MQTTACL_H

#include <list>
#include <string>
#include "MqttCommon.h"

class MqttAcl {

public:
    bool loadAcl(const std::string& aclFile);
    MQTT_ACL_STATE checkAcl(const mqtt_acl_rule_t& rule);
private:
    MQTT_ACL_STATE check_acl_detail(const mqtt_acl_rule_t& acl_rule,const mqtt_acl_rule_t& rule);
private:
    std::list<mqtt_acl_rule_t> acl_;
};


#endif //MQTTSERVER_MQTTACL_H
