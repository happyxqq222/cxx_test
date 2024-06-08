//
// Created by xqq on 24-6-6.
//

#ifndef MQTTSERVER_MQTTSESSION_H
#define MQTTSERVER_MQTTSESSION_H

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <cstdint>
#include <list>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ByteBuffer.h"
#include "MqttCommon.h"
#include "MqttSessionState.h"

using boost::asio::awaitable;

class MqttBroker;
class MqttSession : public std::enable_shared_from_this<MqttSession> {
public:
    MqttSession(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> sslSocket,
                boost::asio::io_context& curIoc, MqttBroker& broker);
    void start();
    std::string& getSessionId();
    void moveSessionState(std::shared_ptr<MqttSession> oldSession);
    void pushPack(const mqtt_packet_t& packet);
    ~MqttSession();

private:
    /**
     * 处理tls握手
     * 
     * @return boost::asio::awaitable<void> 
     */
    boost::asio::awaitable<void> handleHandshake();
    void handleSession();
    void disconnect();
    void initBuffer();
    //收到包了更新deadline用keeplive保活
    void flushDeadline();
    void handleErrorCode();
    uint16_t genPacketId();
    
    /**
     * 
     * 检测ByteBuf的可读长度如果少于该长度就抛出异常
     * @param readableBytes 预期可读字节
     * @param errorMsg 错误描述
     */
    void checkReadableBytes(uint32_t readableBytes,std::string_view errorMsg);
    /**
     * 尝试读取剩余长度
     * @brief
     */
    void readRemainingLength();
    void readCommand();

    /**
     * @brief 
     *  读取剩余消息体
     * @return awaitable<void> 
     */
    awaitable<void> handleRemainingBody();

    awaitable<void> handleKeepAlive();
    awaitable<void> handlePacket();

    awaitable<void> handleConnect();

    awaitable<MQTT_RC_CODE> handlePublish();
    awaitable<MQTT_RC_CODE> handlePuback();
    awaitable<MQTT_RC_CODE> handlePubRec();
    awaitable<MQTT_RC_CODE> handlePubRel();
    awaitable<MQTT_RC_CODE> handlePubComp();
    awaitable<MQTT_RC_CODE> handleSubscribe();
    awaitable<MQTT_RC_CODE> handleUnsubscribe();
    awaitable<MQTT_RC_CODE> handlePingReq();
    awaitable<MQTT_RC_CODE> handleDisconnect();
    awaitable<MQTT_RC_CODE> readByte(uint8_t* addr, bool recordPos);
    awaitable<MQTT_RC_CODE> readUint16(uint16_t* addr, bool recordPos);
    awaitable<MQTT_RC_CODE> readBytesToBuf(std::string& bytes, uint32_t n, bool recordPos);
    awaitable<MQTT_RC_CODE> readUint16HeaderLengthBytes(std::string& bytes, bool record_pos);
    awaitable<MQTT_RC_CODE> readUtf8String(std::string& str, bool record_pos);
    awaitable<MQTT_RC_CODE> readWillPacket(mqtt_packet_t& packet, bool record_pos);

    MQTT_RC_CODE addMqttFixedHeader(std::string& packet, uint8_t cmd, uint32_t remainingLength);
    void checkCommand();
    void checkRemainingLength();
    MQTT_RC_CODE checkSubTopic(const std::string& topicName);
    MQTT_RC_CODE checkPubTopic(const std::string& topicName);
    MQTT_RC_CODE checkValidateUtf8(const std::string& ustr);



    // awaitable<MQTT_RC_CODE> readRemainingLength();
    // awaitable<MQTT_RC_CODE> readFixedHeader();
    awaitable<void> sendConnack(uint8_t ack, uint8_t reasonCode);
    awaitable<MQTT_RC_CODE> sendSuback(uint16_t packetId, const std::string& payload);
    awaitable<MQTT_RC_CODE> sendUnsuback(uint16_t packetId);
    awaitable<MQTT_RC_CODE> sendPuback(uint16_t packetId);
    awaitable<MQTT_RC_CODE> sendPubrec(uint16_t packetId);
    awaitable<MQTT_RC_CODE> sendPubrel(uint16_t packetId);
    awaitable<MQTT_RC_CODE> sendPubcom(uint16_t packetId);
    awaitable<MQTT_RC_CODE> sendPingresp();

    awaitable<MQTT_RC_CODE> handleInflightingPackets();
    awaitable<MQTT_RC_CODE> handleWaitingMapPackets();
    awaitable<MQTT_RC_CODE> sendMqttPackets(const std::list<mqtt_packet_t>& packetList);
    awaitable<void> sendPublishQos0(mqtt_packet_t packet);
    awaitable<MQTT_RC_CODE> sendPublishQos1(mqtt_packet_t packet, bool isNew);
    awaitable<MQTT_RC_CODE> sendPublishQos2(mqtt_packet_t packet, bool isNew);
    void addSubscribe(const std::list<std::pair<std::string, uint8_t>>& subTopicList);

public:
    //统计存在订阅项的会话，优化消息分发的性能
    static std::unordered_set<std::string> activeSubSet;

private:
    class BufReaderMarkRaii {
    public:
        BufReaderMarkRaii(ByteBuffer* buf) : buf_(buf) {
            if (buf_ != nullptr) {
                buf_->markReaderIndex();
            }
        }
        ~BufReaderMarkRaii() {
            if (buf_ != nullptr) {
                buf_->resetReaderIndex();
            }
        }

    private:
        ByteBuffer* buf_;
    };

    enum class ParseState {
        //未开始
        NONE,
        //已经读取第一个字节命令行
        COMMAND, 
        //已经读取可变长度
        REMAINING_LENGTH,
    };

private:
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> sslSocket_;
    boost::asio::io_context& curIoc_;
    std::string username_;
    std::string clientId_;
    MqttBroker& broker_;
    boost::asio::steady_timer condTimer_;
    boost::asio::steady_timer keepAliveTimer_;
    boost::asio::steady_timer checkTimer_;
    MqttSessionState sessionState_;
    bool completeConnect_;
    MQTT_RC_CODE rc_;

    std::vector<char> buf_;
    ByteBuffer recvBuf_;
    
    bool sessionPresent;
    uint8_t command_;
    uint32_t pos_;
    //报文长度
    uint32_t remainingLength_ = 0;
    //存储报文长度字节
    uint8_t remainingCount_ = 0;
    //解析状态
    ParseState parseState_ = ParseState::NONE;
    std::chrono::steady_clock::time_point deadline_;
    //是否
    bool isClose_ = false;
    boost::uuids::random_generator uuidGenerator;
};

#endif    // MQTTSERVER_MQTTSESSION_H
