#include "MqttBroker.h"
#include <string>

using namespace std;

MqttBroker::MqttBroker() :genSidCounter_(0){}

bool MqttBroker::joinOrUpdate(std::shared_ptr<MqttSession> session) {
    bool sessionPresent = false;
    string sid = session->getSessionId();
    auto iter = sessionMap_.find(sid);
    if(iter != sessionMap_.end()) {
        //会话恢复
        auto oldSession = iter->second;
        oldSession->moveSessionState(session);
        sessionPresent = true;
    }
    sessionMap_[sid] = session;
    return sessionPresent;
}

// bool MqttBroker::leave(const std::string& sid) {

// }

// void MqttBroker::dispatch(const mqtt_packet_t& packet) {

// }

// void MqttBroker::dispatchWill(const mqtt_packet_t& packet, const std::string& sid) {

// }

// void MqttBroker::addRetain(const mqtt_packet_t& packet) {

// }

// void MqttBroker::getRetain(std::shared_ptr<MqttSession> session, std::string& topicName) {

// }

// void MqttBroker::removeRetain(const std::string& topicName) {

// }