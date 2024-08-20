//
// Created by xqq on 24-6-6.
//

#include "MqttAcl.h"

#include <yaml-cpp/yaml.h>

#include <boost/regex/v5/regex.hpp>
#include <boost/regex/v5/regex_fwd.hpp>
#include <boost/regex/v5/regex_match.hpp>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <boost/regex.hpp>

#include "MqttCommon.h"
#include "spdlog/spdlog.h"

using namespace std;
using namespace YAML;

bool MqttAcl::loadAcl(const std::string& aclFile) {
    try {
        YAML::Node root = YAML::LoadFile(aclFile);
        for (const auto& config : root) {
            mqtt_acl_rule_t rule;
            if (!config["permission"].IsDefined()) {
                throw std::runtime_error("acl file have no permission");
            }
            std::string permission = config["permission"].as<string>();
            if (permission == "allow") {
                rule.permission = MQTT_ACL_STATE::ALLOW;
            } else if (permission == "deny") {
                rule.permission = MQTT_ACL_STATE::DENY;
            } else {
                throw "acl file permission config error";
            }
            if (!config["type"].IsDefined()) {
                throw "acl file no type";
            }
            string type = config["type"].as<string>();
            if (type == "username") {
                rule.type = MQTT_ACL_TYPE::USERNAME;
            } else if (type == "ipaddr") {
                rule.type = MQTT_ACL_TYPE::IPADDR;
            } else if (type == "clientid") {
                rule.type = MQTT_ACL_TYPE::CLIENTID;
            } else {
                throw "acl file type config error";
            }

            if (!config["object"].IsDefined()) {
                throw "acl file no object";
            }
            rule.object = config["object"].as<std::string>();
            if (!config["mode"].IsDefined()) {
                throw "acl file no mode";
            }
            string mode = config["mode"].as<string>();

            if (mode == "eq") {
                rule.mode = MQTT_ACL_MODE::EQ;
            } else if (mode == "re") {
                rule.mode = MQTT_ACL_MODE::RE;
            } else {
                throw "acl file mode config error";
            }

            if (rule.type == MQTT_ACL_TYPE::USERNAME) {
                if (!config["action"].IsDefined()) {
                    throw "acl file no action";
                }
                string action = config["action"].as<string>();
                if (action == "sub") {
                    rule.action = MQTT_ACL_ACTION::SUB;
                } else if (action == "pub") {
                    rule.action = MQTT_ACL_ACTION::PUB;
                } else if (action == "all") {
                    rule.action = MQTT_ACL_ACTION::ALL;
                } else {
                    throw "acl file action config error";
                }

                if (!config["topics"].IsDefined()) {
                    throw "acl file no topics";
                }
                std::unordered_set<string> st;

                for (const auto& topic : config["topics"]) {
                    st.emplace(topic.as<string>());
                }

                rule.topics = make_unique<unordered_set<string>>(std::move(st));
            }
            acl_.emplace_back(std::move(rule));
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("acl file config error :[{}]", e.what());
        return false;
    }
    return true;
}

MQTT_ACL_STATE MqttAcl::checkAcl(const mqtt_acl_rule_t& rule) {
    for (auto& acl_rule : acl_) {
        auto state = check_acl_detail(acl_rule, rule);
        if(state != MQTT_ACL_STATE::NONE) {
            return state;
        }
    }
    return MQTT_ACL_STATE::NONE;
}

/**
 * @brief 
 * 
 * @param acl_rule  配置文件里的规则
 * @param rule 需要匹配的规则
 * @return MQTT_ACL_STATE 
 */
MQTT_ACL_STATE MqttAcl::check_acl_detail(const mqtt_acl_rule_t& acl_rule, const mqtt_acl_rule_t& rule) {
    if(rule.type != acl_rule.type) {
        return MQTT_ACL_STATE::NONE;
    }

    if(acl_rule.mode == MQTT_ACL_MODE::EQ) {
        if(acl_rule.object != rule.object) {
            return MQTT_ACL_STATE::NONE;
        }
    } else {
        try{
            boost::regex expr{acl_rule.object};
            if(!boost::regex_match(rule.object,expr)) {
                return MQTT_ACL_STATE::NONE;
            }
        } catch (std::exception& e){
            SPDLOG_ERROR("regex_match error : [{}]" , e.what());
            return MQTT_ACL_STATE::NONE;
        }
    }

    if(acl_rule.type == MQTT_ACL_TYPE::USERNAME) {
        if(acl_rule.action != MQTT_ACL_ACTION::ALL &&
           acl_rule.action != rule.action) {
            return MQTT_ACL_STATE::NONE;
        }

        auto topic = *(rule.topics->begin());

        //对于订阅控制来说只需要进行精确控制
        if(rule.action == MQTT_ACL_ACTION::SUB) {
            if(acl_rule.topics->contains(topic)) {
                return acl_rule.permission;
            }
            return MQTT_ACL_STATE::NONE;
        }

        //对于发布控制来说需要进行通配符匹配
        for(const auto& t : *(acl_rule.topics)) {
            if(util::check_topic_match(topic, t)){
                return acl_rule.permission;
            }
        }
        return MQTT_ACL_STATE::NONE;
    } else if (acl_rule.type == MQTT_ACL_TYPE::IPADDR) {
        return acl_rule.permission;
    } else if(acl_rule.type == MQTT_ACL_TYPE::CLIENTID) {
        return acl_rule.permission;
    }
    return MQTT_ACL_STATE::NONE;
}
