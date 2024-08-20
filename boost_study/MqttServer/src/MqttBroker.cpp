#include "MqttBroker.h"

#include <memory>
#include <string>

#include "MqttSession.h"
#include "spdlog/spdlog.h"

using namespace std;

MqttBroker::MqttBroker() : genSidCounter_(0) {}

bool MqttBroker::joinOrUpdate(std::shared_ptr<MqttSession> session) {
    bool sessionPresent = false;
    string sid = session->getSessionId();
    SessionMap::accessor findAcc;
    //使用raii来自动释放
    unique_ptr<SessionMap::accessor, SessionMapAccessorDeleter> findAccPtr(&findAcc);
    if (sessionMap_.find(findAcc, sid)) {
        //会话恢复
        auto oldSession = findAcc->second;
        //会话状态恢复
        oldSession->moveSessionState(session);
        findAcc->second = session;
        sessionPresent = true;
    } else {
        SessionMap::accessor insertAcc;
        unique_ptr<SessionMap::accessor, SessionMapAccessorDeleter> insertAccPtr(&insertAcc);
        sessionMap_.insert(insertAcc, sid);
        insertAcc->second = session;
    }

    return sessionPresent;
}

void MqttBroker::leave(const std::string& sid) { sessionMap_.erase(sid); }

void MqttBroker::dispatch(const mqtt_packet_t& packet) {
    SPDLOG_DEBUG("dispatch packet, session_map size=[{}]", sessionMap_.size());
    //将消息分发给拥有订阅项的会话
    for (auto& sid : MqttSession::activeSubSet) {
        SessionMap::accessor findAcc;
        unique_ptr<SessionMap::accessor, SessionMapAccessorDeleter> findAccPtr(&findAcc);
        bool findResult = sessionMap_.find(findAcc, sid);
        if (!findResult) {
            continue;
        }
        findAcc->second->pushPacket(packet);
    }
}

void MqttBroker::dispatchWill(const mqtt_packet_t& packet, const std::string& sid) {
    //遗嘱消息不发送给已经死去的会话，虽然死了但还是可能保留了会话
    //会继续接收主题消息
    for (auto& sessionId : MqttSession::activeSubSet) {
        if (sessionId != sid) {
            SessionMap::accessor findAcc;
            unique_ptr<SessionMap::accessor, SessionMapAccessorDeleter> findAccPtr(&findAcc);
            bool findResult = sessionMap_.find(findAcc, sid);
            if (!findResult) {
                continue;
            }
            findAcc->second->pushPacket(packet);
        }
    }
}

void MqttBroker::addRetain(const mqtt_packet_t& packet) {
    RetainMap::accessor findAcc;
    unique_ptr<RetainMap::accessor,RetainMapAccessorDeleter> findAccPtr(&findAcc);
    bool findResult = retainMap_.find(findAcc,*packet.topic_name);
    if(findResult) {
        findAcc->second = packet;
    }
}

void MqttBroker::getRetain(std::shared_ptr<MqttSession> session, const std::string& subTopic) {
    for(auto& [pubTopic,packet] : retainMap_) {
        //指定消息要发送给哪个主题
        packet.specifiedTopicName = subTopic;
        session->pushPacket(packet);
    }
}

void MqttBroker::removeRetain(const std::string& topicName) {
    retainMap_.erase(topicName);
}