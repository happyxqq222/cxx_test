//
// Created by xqq on 24-6-6.
//

#include "MqttSession.h"

#include <oneapi/tbb/concurrent_set.h>
#include <pthread.h>

#include <algorithm>
#include <array>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/stream_base.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/detail/frame_unwind.ipp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ByteBuffer.h"
#include "MqttBroker.h"
#include "MqttCommon.h"
#include "MqttConfig.h"
#include "MqttException.h"
#include "spdlog/fmt/bundled/format.h"
#include "spdlog/spdlog.h"

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace boost;
using namespace boost::asio;
using namespace boost::stacktrace;
using namespace boost::system;

std::unordered_set<std::string> MqttSession::activeSubSet;

MqttSession::MqttSession(
    unique_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>& sslSocket,
    io_context& curIoc, MqttBroker* broker, bool& isClose)
    : sslSocket_(std::move(sslSocket)),
      curIoc_(curIoc),
      broker_(broker),
      condTimer_(sslSocket_->get_executor()),
      keepAliveTimer_(sslSocket_->get_executor()),
      checkTimer_(sslSocket_->get_executor()),
      completeConnect_(false),
      rc_(MQTT_RC_CODE::ERR_SUCCESS),
      buf_(1024, 0),
      command_(0),
      pos_(0),
      isClose_(isClose) {
    SPDLOG_INFO("sslSocket:{}", static_cast<void*>(sslSocket_.get()));
    this->condTimer_.expires_at(steady_clock::time_point::max());
    this->keepAliveTimer_.expires_at(steady_clock::time_point::max());
    this->checkTimer_.expires_at(steady_clock::time_point::max());
}

void MqttSession::start() {
    address_ = sslSocket_->next_layer().remote_endpoint().address().to_string();
    SPDLOG_INFO("new connection:{}", address_);
    co_spawn(
        curIoc_,
        [self = shared_from_this()]() -> awaitable<void> { return self->handleHandshake(); },
        detached);
}

ssl::stream<ip::tcp::socket>& MqttSession::getSslSocket() { return *sslSocket_; }

awaitable<void> MqttSession::handleHandshake() {
    try {
        SPDLOG_INFO("start handleshake :{}", pthread_self());
        //这地方会等握手返回然后再执行handlSession.同时协程上下文会调度执行别协程代码
        co_await sslSocket_->async_handshake(ssl::stream_base::server, use_awaitable);
        SPDLOG_INFO("ssl handleshake success!");
        // tls协商密钥成功 开始执行session
        handleSession();
    } catch (std::exception& e) {
        SPDLOG_ERROR("handleshake failed : [{}]", e.what());
        disconnect();
    }
}

void MqttSession::handleSession() {
    // CONNECT 阶段超时时间
    this->sessionState_.keepAlive_ = MqttConfig::getInstance()->connect_timeout();

    //启动一个协程解析包
    asio::co_spawn(
        curIoc_, [self = shared_from_this()] { return self->handlePacket(); }, detached);

    //启动一个协程处理keepalive
    // asio::co_spawn(
    //     curIoc_, [self = shared_from_this()] { return self->handleKeepAlive(); }, detached);
}

void MqttSession::disconnect() {
    system::error_code ignoredEc;
    this->sslSocket_->shutdown(ignoredEc);
    this->sslSocket_->next_layer().close(ignoredEc);
    this->condTimer_.cancel(ignoredEc);
    this->keepAliveTimer_.cancel(ignoredEc);
    this->checkTimer_.cancel(ignoredEc);
    this->broker_->leave(clientId_);
}

boost::asio::awaitable<void> MqttSession::handleKeepAlive() {
    auto checkDuration = seconds(MqttConfig::getInstance()->check_timeout_duration());

    while (this->sslSocket_->next_layer().is_open()) {
        this->keepAliveTimer_.expires_after(checkDuration);

        co_await this->keepAliveTimer_.async_wait(use_awaitable);

        if (this->deadline_ <= steady_clock::now()) {
            disconnect();
        }
    }
}

void MqttSession::initBuffer() {
    this->rc_ = MQTT_RC_CODE::ERR_SUCCESS;
    this->pos_ = 0;
    this->command_ = 0;
    this->remainingLength_ = 0;
}

boost::asio::awaitable<void> MqttSession::handlePacket() {
    //一个新的协程解析包
    while (!isClose_) {
        try {
            size_t n = co_await sslSocket_->async_read_some(buffer(buf_.data(), buf_.size()),
                                                            use_awaitable);
            if (n == 0) {
                SPDLOG_INFO("peer closed remote ip:{}",
                            sslSocket_->next_layer().remote_endpoint().address().to_string());
                throw MqttException((int)MQTT_RC_CODE::ERR_PEER_CLOSE, "receive peer closed");
            } else {
                SPDLOG_INFO("receive data size:{}", n);
                recvBuf_.append(buf_.data(), n);
                if (recvBuf_.readableBytes() > 0) {
                    if (parseState_ == ParseState::NONE) {
                        readCommand();
                    }
                    if (parseState_ == ParseState::COMMAND) {
                        readRemainingLength();
                    }
                    if (parseState_ == ParseState::REMAINING_LENGTH) {
                        if (recvBuf_.readableBytes() >= 1 + remainingCount_ + remainingLength_) {
                            //先把buf里的COMMAND和剩余长度读取掉
                            recvBuf_.retrieve(1 + remainingCount_);
                            co_await handleCommand();
                        }
                    }
                }
            }

        } catch (const MqttException& e) {
            SPDLOG_INFO("mqtt exception :{} trace:{}", e.what(), e.getStackTrace());
            disconnect();
            break;
        } catch (std::exception& e) {
            SPDLOG_INFO("read error:{}", e.what());
            disconnect();
            break;
        }
    }
    co_return;
}

void MqttSession::flushDeadline() {
    if (this->sessionState_.keepAlive_ > 0) {
        uint16_t keepAlive = this->sessionState_.keepAlive_ * 3 / 2;
        this->deadline_ = steady_clock::now() + seconds(keepAlive);
    }
}

void MqttSession::checkReadableBytes(uint32_t readableBytes, std::string_view errorMsg) {
    if (recvBuf_.readableBytes() < readableBytes) {
        SPDLOG_ERROR("expected readableBytes:{},really readableBytes:{}", readableBytes,
                     recvBuf_.readableBytes());
        throw MqttException((int)MQTT_RC_CODE::ERR_REMAINING_LENGTH, errorMsg);
    }
}

void MqttSession::readCommand() {
    this->command_ = recvBuf_.peekInt8();
    SPDLOG_DEBUG("command = [X'{:02X}']", static_cast<uint16_t>(command));
    checkCommand();
    parseState_ = ParseState::COMMAND;
}

void MqttSession::checkCommand() {
    SPDLOG_INFO("command :{}", command_ & 0xF0);
    switch (command_ & 0xF0) {
        case MQTT_CMD::CONNECT:
        case MQTT_CMD::CONNACK:
        case MQTT_CMD::PUBACK:
        case MQTT_CMD::PUBREC:
        case MQTT_CMD::PUBCOMP:
        case MQTT_CMD::SUBACK:
        case MQTT_CMD::UNSUBACK:
        case MQTT_CMD::PINGREQ:
        case MQTT_CMD::PINGRESP:
        case MQTT_CMD::DISCONNECT: {
            if ((command_ & 0x0F) != 0x00) {
                throw MqttException((int)MQTT_RC_CODE::ERR_COMMAND_RESERVED, "后四位不是0x00");
            }
            break;
        }
        case MQTT_CMD::PUBLISH: {
            break;
        }
        case MQTT_CMD::PUBREL:
        case MQTT_CMD::SUBSCRIBE:
        case MQTT_CMD::UNSUBSCRIBE: {
            if ((command_ & 0x0F) != 0x02) {
                throw MqttException((int)MQTT_RC_CODE::ERR_COMMAND_RESERVED, "后四位命令不是0x02");
            }
            break;
        }
        default: {
            throw MqttException((int)MQTT_RC_CODE::ERR_COMMAND, "错误的命令");
        }
    }

    if ((this->command_ & 0xF0) == MQTT_CMD::CONNECT && this->completeConnect_) {
        throw MqttException((int)MQTT_RC_CODE::ERR_DUP_CONNECT, "重复的连接");
    }

    if ((this->command_ & 0xF0) != MQTT_CMD::CONNECT && !this->completeConnect_) {
        throw MqttException((int)MQTT_RC_CODE::ERR_NOT_CONNECT, "还没建立连接");
    }
    SPDLOG_INFO("checkcommand complete");
}

void MqttSession::readRemainingLength() {
    // 0x80 10000000
    // 0x7F 01111111
    bool isSuccess = false;
    if (recvBuf_.readableBytes() < 2) {
        return;
    } else {
        // recvBuf_.markReaderIndex();
        BufReaderMarkRaii br(&recvBuf_);
        //先读取一个固定头第一个字节
        recvBuf_.readInt8();
        //长度
        vector<uint8_t> remainingLength;
        //存储长度的字节数
        int remainingCount = 1;
        for (;;) {
            //可读字节不够跳出循环，读取长度失败
            if (recvBuf_.readableBytes() == 0) {
                isSuccess = false;
                break;
            }
            uint8_t byte = recvBuf_.readInt8();
            if (byte & 0x80) {
                // byte & 0x7F 去掉最高位(继续位)
                remainingLength.push_back(byte & 0x7F);
            } else {    //继续位 为0
                //读取结束
                isSuccess = true;
                remainingLength.push_back(byte & 0x7F);
                SPDLOG_INFO("remainingCount:{}", remainingCount);
                break;
            }

            remainingCount++;

            if (remainingCount == 5) {
                //存储长度字节最多是四个 超过五出现错误
                SPDLOG_DEBUG("read remaining length error");
                throw MqttException((int)MQTT_RC_CODE::ERR_REMAINING_LENGTH,
                                    "remainingLength is invalid!");
            }
        }

        if (isSuccess) {
            int len = 0;
            for (uint8_t idx = 0; idx < remainingCount; idx++) {
                len = len + remainingLength[idx] * pow(128, idx);
            }
            this->remainingCount_ = remainingCount;
            this->remainingLength_ = len;
            SPDLOG_INFO("remaining length = [{}]", len);
            checkRemainingLength();
            parseState_ = ParseState::REMAINING_LENGTH;
            return;
        } else {
            this->remainingCount_ = 0;
            this->remainingLength_ = 0;
            SPDLOG_INFO("read remaining length error");
            return;
        }
    }
}

void MqttSession::checkRemainingLength() {
    SPDLOG_INFO("检测报文长度");
    switch (this->command_ & 0xF0) {
        case MQTT_CMD::PUBACK:
        case MQTT_CMD::PUBREC:
        case MQTT_CMD::PUBREL:
        case MQTT_CMD::PUBCOMP:
        case MQTT_CMD::UNSUBACK: {
            if (remainingLength_ != 2) {
                throw MqttException((int)MQTT_RC_CODE::ERR_MALFORMED_PACKET,
                                    "报文长度错误,长度必须为2");
            }
            break;
        }
        case MQTT_CMD::PINGREQ:
        case MQTT_CMD::PINGRESP:
        case MQTT_CMD::DISCONNECT: {
            if (this->remainingLength_ != 0) {
                throw MqttException((int)MQTT_RC_CODE::ERR_MALFORMED_PACKET,
                                    "报文长度错误，报文长度必须为0");
            }
        }
        default: {
            break;
        }
    }
}

awaitable<void> MqttSession::handleCommand() {
    SPDLOG_INFO("start handle {} ", command_ & 0xF0);
    switch (command_ & 0xF0) {
        case MQTT_CMD::CONNECT:
            co_await handleConnect();
            break;
        case MQTT_CMD::PUBLISH:
            co_await handlePublish();
            break;
        case MQTT_CMD::SUBSCRIBE:
            break;
        case MQTT_CMD::UNSUBSCRIBE:
            break;
    }
}

awaitable<void> MqttSession::handleConnect() {
    SPDLOG_INFO("start handle connect");
    //清理会话标志
    bool cleanSession;
    /**
     * 遗嘱标志位 当客户端意外断开连接，服务器将发布遗嘱消息，通知其他客户端该客户端已经断开
     * 1: 启用遗嘱
     * 0: 禁用遗嘱消息
     * 当 qos为1时 connect报文中必须包含以下遗嘱相关字段
     * 1.遗嘱 QoS（Will QoS）：2 位，用于指定遗嘱消息的服务质量（QoS）级别。
     * 2.遗嘱保留（Will Retain）：1 位，用于指定遗嘱消息是否需要保留。
     * 3.遗嘱主题（Will Topic）：表示遗嘱消息发布的主题。
     * 4.遗嘱消息（Will Message）：表示在客户端意外断开连接时发布的消息内容。
     */
    bool willFlag;

    mqtt_packet_t willTopic;
    uint8_t passwordFlag, usernameFlag;
    string username, password;
    uint16_t keepAlive;
    string clientId;

    //读取可变报头
    // 1.读取协议名称长度
    checkReadableBytes(2, "[conn] read protocolLen remaining length is error");
    uint16_t protocolNameLen = recvBuf_.readInt16();
    spdlog::info("CONNECT: read protocol name length :{}", protocolNameLen);
    if (protocolNameLen != 4) {
        SPDLOG_ERROR("[conn] protocolName length must be 4 but is {}", protocolNameLen);
        throw MqttException((int)MQTT_RC_CODE::ERR_PROTOCOL,
                            "[conn] protocolName length must be 4");
    }

    // 2.读取协议名称
    checkReadableBytes(protocolNameLen, "[conn] read protocolName remaining length is error");
    string protocolName = recvBuf_.retrieveAsString(protocolNameLen);
    spdlog::info("CONNECT: read protocol name:{}", protocolName);

    if (protocolName != "MQTT") {
        SPDLOG_ERROR("protocol_name is error = {}", protocolName);
        throw MqttException((int)MQTT_RC_CODE::ERR_PROTOCOL, "[conn] protocol name must be MQTT");
    }

    // 3.读取协议版本
    checkReadableBytes(1, "[conn] read protocolVersion remaining length is error");
    uint8_t protocolVersion = recvBuf_.readInt8();
    spdlog::info("protocol version:{}", protocolVersion);
    // MQTT 3.1.1
    if (protocolVersion != 0x04) {
        SPDLOG_ERROR("protocol version is error = {}", protocolVersion);
        //协议名称错误 发送connAck
        co_await sendConnack(0x00, MQTT_CONNACK::REFUSED_PROTOCOL_VERSION);
        throw MqttException((int)MQTT_RC_CODE::ERR_PROTOCOL, "[conn] protocol version is error");
    }

    // 4.读取标志位
    checkReadableBytes(1, "[conn] read connect flag remaining length is error");
    uint8_t connectFlag = recvBuf_.readInt8();
    SPDLOG_INFO("connect flag:{}", connectFlag);
    cleanSession = (connectFlag & 0x02) >> 1;
    willFlag = connectFlag & 0x04;
    willTopic.qos = (connectFlag & 0x18) >> 3;
    willTopic.retain = ((connectFlag & 0x20) == 0x20);
    passwordFlag = connectFlag & 0x40;
    usernameFlag = connectFlag & 0x80;

    //遗嘱保留级别为3 无效
    if (willTopic.qos == 3) {
        SPDLOG_ERROR("will qos is invalid qos: {}", (uint8_t)willTopic.qos);
        throw MqttException((int)MQTT_RC_CODE::ERR_PROTOCOL, "[conn] will qos is invalid");
    }

    // 如果禁用遗嘱时(willFlag = false) 遗嘱qos必须为0而且遗嘱消息不能设置为保留
    if (willFlag == false && (willTopic.qos != 0 || willTopic.retain)) {
        SPDLOG_ERROR(
            "if testament is disable qos shuould be 0 and retain is disable ,qos:{},retain:{}",
            (uint8_t)willTopic.qos, (uint8_t)willTopic.retain);
        throw MqttException(
            (int)MQTT_RC_CODE::ERR_PROTOCOL,
            "[conn] if testament is disable qos shuould be 0 and retain is disable");
    }

    // 4.读取keepAlive
    checkReadableBytes(2, "[conn] read keepalive  remaining length is error");
    keepAlive = recvBuf_.readInt16();
    spdlog::info("[conn] keepalive:{}", keepAlive);

    // 5.读取客户端标识长度
    checkReadableBytes(2, "[conn] read clientid length  remaining length is error");
    uint16_t clientIdLen = recvBuf_.readInt16();

    // mqtt 3.1.1 如果客户端标识符长度为 0，清理会话标志必须设置为 1（True）
    if (clientIdLen == 0 && !cleanSession) {
        SPDLOG_ERROR("when clientIdLen equals 0 the clientSession must be set true");
        co_await sendConnack(0x00, MQTT_CONNACK::REFUSED_IDENTIFIER_REJECTED);
        throw MqttException((int)MQTT_RC_CODE::ERR_PROTOCOL,
                            "[conn] when clientIdLen equals 0 the clientSession must be set true");
    }

    // 6.读取客户端id
    checkReadableBytes(clientIdLen, "[conn] read clientid  remaining length is error");
    clientId = recvBuf_.retrieveAsString(clientIdLen);
    spdlog::info("clientid:{}", clientId);

    // clientid_ 前缀用于自动生成的 client_id, 客户端生成的不能带有 clientid_ 前缀
    if (clientId.starts_with("clientid_")) {
        SPDLOG_ERROR("clientid can't start with clientid_");
        co_await sendConnack(0x00, MQTT_CONNACK::REFUSED_IDENTIFIER_REJECTED);
        throw MqttException((int)MQTT_RC_CODE::ERR_BAD_CLIENT_ID,
                            "[conn] clientid can't start with clientid_");
    }

    if (clientId.empty()) {
        //生成clientid
        clientId = "clientid_" + boost::uuids::to_string(uuidGenerator());
    }

    //读取遗嘱消息
    if (willFlag) {
        // 7.读取遗嘱主题长度
        checkReadableBytes(2, "[conn] read topicName len  remaining length is error");
        uint16_t topicNameLen = recvBuf_.readInt16();

        // 8.读取遗嘱主题
        checkReadableBytes(topicNameLen, "[conn] read topicName  remaining length is error");
        string topicName = recvBuf_.retrieveAsString(topicNameLen);

        // 9.读取遗嘱消息长度
        checkReadableBytes(2, "[conn] read paylod len  remaining length is error");
        uint16_t payloadLen = recvBuf_.readInt16();

        // 10.读取遗嘱消息
        checkReadableBytes(payloadLen, "[conn] read paylod  remaining length is error");
        string paylod = recvBuf_.retrieveAsString(payloadLen);

        willTopic.topic_name = std::make_shared<const string>(std::move(topicName));
        willTopic.payload = std::make_shared<const string>(std::move(paylod));
    }

    // 11.读取用户名
    if (usernameFlag) {
        checkReadableBytes(2, "[conn] read username len  remaining length is error");
        uint16_t userNameLen = recvBuf_.readInt16();
        checkReadableBytes(userNameLen, "[conn] read username  remaining length is error");
        username = recvBuf_.retrieveAsString(userNameLen);
    }

    // 12.读取密码
    if (passwordFlag) {
        checkReadableBytes(2, "[conn] read password len  remaining length is error");
        uint16_t passwordLen = recvBuf_.readInt16();
        checkReadableBytes(passwordLen, "[conn] read password  remaining length is error");
        password = recvBuf_.retrieveAsString(passwordLen);
    }

    //进行密码校验
    if (MqttConfig::getInstance()->auth()) {
        if (!MqttConfig::getInstance()->auth(username, password)) {
            SPDLOG_ERROR("username or password is incorrect username:{},password:{}", username,
                         password);
            co_await sendConnack(0x00, MQTT_CONNACK::REFUSED_BAD_USERNAME_PASSWORD);
            throw MqttException((int)MQTT_RC_CODE::ERR_BAD_USERNAME_PASSWORD,
                                "[conn] username or password is incorrect");
        }
        this->username_ = username;
    }

    //进行ACL校验
    if (MqttConfig::getInstance()->acl_enable()) {
        mqtt_acl_rule_t rule;
        rule.type = MQTT_ACL_TYPE::IPADDR;
        system::error_code ec;
        rule.object = this->sslSocket_->next_layer().remote_endpoint(ec).address().to_string();
        if (ec) {
            throw MqttException((int)MQTT_RC_CODE::ERR_NO_CONN, "[conn] error connection");
        }
        SPDLOG_DEBUG("remote ip addr : [{}]", rule.object);

        if (!MqttConfig::getInstance()->acl_check(rule)) {
            SPDLOG_ERROR("ip is deny ip:{}", rule.object);
            co_await sendConnack(0x00, MQTT_CONNACK::REFUSED_NOT_AUTHORIZED);
            throw MqttException((int)MQTT_RC_CODE::ERR_REFUSED_NOT_AUTHORIZED,
                                "[conn] ip is deny ip:" + rule.object);
        }

        rule.type = MQTT_ACL_TYPE::CLIENTID;
        rule.object = clientId;

        if (!MqttConfig::getInstance()->acl_check(rule)) {
            SPDLOG_ERROR("clientid is deny clientid:{}", rule.object);
            co_await sendConnack(0x00, MQTT_CONNACK::REFUSED_NOT_AUTHORIZED);
            throw MqttException((int)MQTT_RC_CODE::ERR_REFUSED_NOT_AUTHORIZED,
                                "[conn] clientit is deny clientid:" + rule.object);
        }
    }

    //记录连接状态
    this->clientId_ = clientId;
    this->sessionState_.cleanSession_ = cleanSession;
    this->sessionState_.keepAlive_ = keepAlive;
    this->sessionState_.willTopic = willTopic;

    //加入broker
    sessionPresent = this->broker_->joinOrUpdate(shared_from_this());

    //发送CONNACK响应
    co_await sendConnack(sessionPresent, MQTT_CONNACK::ACCEPTED);

    // connect完成标志设置
    this->completeConnect_ = true;
    //添加自动订阅
    addSubscribe(MqttConfig::getInstance()->auto_subscribe_list());

    //开启协程用于处理需要当前会话转发的主题
    asio::co_spawn(
        curIoc_,
        [self = shared_from_this()]() -> awaitable<void> {
            // return self->handleInflightingPackets();
            co_return;
        },
        detached);

    //开启协程用于处理需要等待一段时间的操作
    co_spawn(
        curIoc_,
        [self = shared_from_this()]() -> awaitable<void> {
            // return self->handleWaitingMapPackets();
            co_return;
        },
        detached);

    SPDLOG_DEBUG("success to handle 'CONNECT', clientid=[{}] cleanSession=[{}]", this->clientId_,
                 this->sessionState_.cleanSession_);
}

std::string& MqttSession::getSessionId() { return this->clientId_; }

void MqttSession::moveSessionState(std::shared_ptr<MqttSession> newSession) {
    //连接完成标志设置为未完成，这样连接断开后不会再去调用leave删除会话
    //也不会发生遗嘱消息
    this->completeConnect_ = false;

    //关闭旧会话连接
    disconnect();

    //恢复会话状态
    if (newSession->sessionState_.cleanSession_ == 0) {
        newSession->sessionState_.inflightQueue_ = std::move(this->sessionState_.inflightQueue_);
        newSession->sessionState_.subTopicMap_ = std::move(this->sessionState_.subTopicMap_);
        newSession->sessionState_.waitingMap_ = std::move(this->sessionState_.waitingMap_);
    }
}

awaitable<void> MqttSession::sendConnack(uint8_t ack, uint8_t reasonCode) {
    SPDLOG_DEBUG("CONNACK: send ack=[X'{:02X}'] reasonCode=[X'{:02X}']", static_cast<uint16_t>(ack),
                 static_cast<uint16_t>(reasonCode));
    std::string packet;
    addMqttFixedHeader(packet, MQTT_CMD::CONNACK, 2);
    std::array<asio::const_buffer, 3> buf = {buffer(packet.data(), packet.length()),
                                             buffer(&ack, sizeof(uint8_t)),
                                             buffer(&reasonCode, sizeof(uint8_t))};
    try {
        co_await async_write(*sslSocket_, buf, use_awaitable);
    } catch (std::exception& e) {
        throw MqttException((int)MQTT_RC_CODE::ERR_NO_CONN,
                            "send connack error where call async_write");
    }
    co_return;
}

void MqttSession::addMqttFixedHeader(std::string& packet, uint8_t cmd, uint32_t remainingLength) {
    //使用连除法计算128进制
    uint8_t byte;    //存储每次连除所产生的余数
    vector<uint8_t> remaingBytes;

    for (;;) {
        byte = remainingLength % 0x80;
        remainingLength = remainingLength / 0x80;
        remaingBytes.push_back(byte);
        if (remainingLength == 0) {
            break;
        }
    }

    if (remaingBytes.size() > 4) {
        SPDLOG_ERROR("reaming length should less than 5 but is {}", remaingBytes.size());
        throw MqttException(
            (int)MQTT_RC_CODE::ERR_PAYLOAD_SIZE,
            "reaming length should less than 5 but is " + to_string(remaingBytes.size()));
    }

    packet.reserve(1 + remaingBytes.size());
    //命令行插入
    packet.push_back(cmd);
    packet[0] = cmd;
    //将128个进制剩余长度写入到packet中
    packet.insert(1, reinterpret_cast<const char*>(remaingBytes.data()), remaingBytes.size());
}

void MqttSession::addSubscribe(const std::list<std::pair<std::string, uint8_t>>& subTopicList) {
    if (subTopicList.empty()) {
        return;
    }

    //更新存在订阅的会话
    addSubscribedClientid(this->clientId_);


    for (const auto& [subTopic, qos] : subTopicList) {
        SPDLOG_DEBUG("subscribe topic name = [{}], qos = [X'{:02X}']", subTopic,
                     static_cast<uint16_t>(qos));
        this->sessionState_.subTopicMap_[subTopic] = qos;

        //获取保留消息，只针对当前新增主题
        this->broker_->getRetain(shared_from_this(), subTopic);
    }
}

void MqttSession::pushPacket(const mqtt_packet_t& packet) {
    this->sessionState_.inflightQueue_.emplace(packet);
    SPDLOG_DEBUG("pubsh packet: topic_name = [{}],payload = [{}]", *packet.topic_name,
                 *packet.paylod);
    //通知处理线程读取消息
    this->condTimer_.cancel_one();
}

void MqttSession::addSubscribedClientid(const std::string clientId) {
    std::lock_guard<std::mutex> lock(activeSubSetMtx);
    activeSubSet.insert(clientId);
    
}
void MqttSession::removeSubscribedClientId(const std::string clientId) {
    unique_lock<mutex> lock(activeSubSetMtx);
    activeSubSet.erase(clientId);
}

awaitable<void> MqttSession::handleInflightingPackets() {
    mqtt_packet_t packet;
    uint8_t oldQos;
    std::list<mqtt_packet_t> sendPacketList;
    while (this->sslSocket_->next_layer().is_open()) {
        packet = this->sessionState_.inflightQueue_.front();
        for(auto& [subTopic,qos] : this->sessionState_.subTopicMap_) {
            //保留消息指定具体的主题名称
            if(packet.retain && packet.specifiedTopicName.length() && subTopic != packet.specifiedTopicName) {
                continue;
            }
            if(util::check_topic_match(*packet.topic_name, subTopic)) {
                //Qos等级取两者最小值
                oldQos = packet.qos;
                packet.qos = std::min(packet.qos,qos);
                sendPacketList.emplace_back(packet);
                packet.qos = oldQos;
            }
        }
        //批量发送订阅消息
        co_await sendMqttPackets(sendPacketList);
        sendPacketList.clear();
    }
}

awaitable<void> MqttSession::sendMqttPackets(const std::list<mqtt_packet_t>& packetList) {
    if(packetList.empty()) {
        co_return;
    }

}

awaitable<void> MqttSession::handlePublish() {
    mqtt_packet_t pubPacket;
    uint8_t retain = this->command_ & 0x01;
    uint8_t qos = (this->command_ >> 1) & 0x03;
    uint8_t dup = (this->command_ >> 3) & 1;
    std::shared_ptr<string> pubTopic;
    uint16_t packetId;
    std::shared_ptr<string> pubPayload;

    //qos只有0 1 2  qos为0(最多传一次) dup不能为1
    if(qos == 3 || (qos == 0 && dup == 1)) {
        throw MqttException((int)MQTT_RC_CODE::ERR_MALFORMED_PACKET,fmt::format("qos={} ,dup={}",qos,dup));
    }

    int pubTopicLen = recvBuf_.readInt16();
    checkReadableBytes(pubTopicLen, "[conn] read clientid  remaining length is error");
    pubTopic = make_shared<string>(recvBuf_.retrieveAsString(pubTopicLen));
    spdlog::info("publish topic:{}",*pubTopic);
    checkPubTopic(*pubTopic);
    
    if(qos > 0) {
        packetId =  recvBuf_.readInt16();
        if(packetId == 0) {
            throw MqttException((int)MQTT_RC_CODE::ERR_PROTOCOL,fmt::format("packetid id invalid :{}",packetId));
        }
    }

    pubPayload =  make_shared<string>(recvBuf_.retrieveAllAsString());
    
    //报文读取完毕,对于qos1 和qos2 级别需要发送响应报文
    if(qos == 1) {
        co_await sendPuback(packetId);
    } else if(qos == 2) {
        co_await sendPubrec(packetId);
        //对于qos2几倍，如果客户端没有收到PUBREC报文，可能会重传
        //因此只要packetId还没被服务端释放，就不再接受重传报文
        //保证一个消息到达
        if(dup == 1 && this->sessionState_.waitingMap_.contains(packetId)) {
            co_return;
        }

        //等待PUBREL
        mqtt_packet_t packet;
        packet.state = MQTT_MSG_STATE::WAIT_RECEIVE_PUBREL;
        packet.expire_time = std::chrono::steady_clock::now() + 
                             std::chrono::seconds(MqttConfig::getInstance()->max_waiting_time());
        this->sessionState_.waitingMap_[packetId] = std::move(packet);
    }

    //ACL检查
    if(MqttConfig::getInstance()->acl_enable()) {
        mqtt_acl_rule_t rule;
        rule.type = MQTT_ACL_TYPE::USERNAME;
        rule.object = this->username_;
        rule.action = MQTT_ACL_ACTION::PUB;
        rule.topics = std::make_unique<std::unordered_set<std::string>>();
        rule.topics->emplace(pubTopic);
        if(!MqttConfig::getInstance()->acl_check(rule)) {
            throw MqttException((int)MQTT_RC_CODE::ERR_SUCCESS, "acl ");
        }
    }

    //组包
    pubPacket.dup = dup;
    pubPacket.qos = qos;
    pubPacket.retain = retain;
    pubPacket.topic_name = pubTopic;
    pubPacket.payload = pubPayload;

    //添加到保留信息
    if(retain) {
        //内容为空则移除保留消息
        if(pubPayload->empty()) {
            this->broker_->removeRetain(*pubPacket.topic_name);
        } else {
            this->broker_->addRetain(pubPacket);
        }
    }

    pubPacket.dup = 0;
    pubPacket.retain = 0;
     
    //消息分发
    this->broker_->dispatch(pubPacket);
    co_return;
}

void MqttSession::checkPubTopic(const std::string& topicName) {
    if(topicName.empty()) {
        throw MqttException((int)MQTT_RC_CODE::ERR_MALFORMED_PACKET,"topicName should be not empty");
    }

    if(topicName.length() > 65535) {
        throw MqttException((int)MQTT_RC_CODE::ERR_STR_LENGTH_UTF8, fmt::format("publishTopic length is invalid,len:{}",topicName.length()));
    }

    //不能向包含通配符# +的主题发布消息
    size_t pos1 = topicName.find('#');
    size_t pos2 = topicName.find('+');
    if(pos1 != string::npos || pos2 != string::npos) {
        throw MqttException((int)MQTT_RC_CODE::ERR_PUB_TOPIC_NAME,fmt::format("publish topic contains # or +, topic:{}",topicName));
    }

}

awaitable<void> MqttSession::sendPuback(uint16_t packetId) {
    string packet;
    uint16_t netPacketId;
    addMqttFixedHeader(packet, MQTT_CMD::PUBACK, 2);
    netPacketId = asio::detail::socket_ops::host_to_network_short(packetId);
    std::array<const_buffer,2> buf = {
        asio::buffer(packet.data(),packet.length()),
        asio::buffer(&netPacketId,sizeof(uint16_t))
    };

    co_await async_write(*this->sslSocket_,buf,asio::use_awaitable);

    spdlog::info("puback: send packetid = [X'{:04X}']",packetId);
}

awaitable<void> MqttSession::sendPubrec(uint16_t packetId) {
    string packet;
    uint16_t netPacketId;
    addMqttFixedHeader(packet, MQTT_CMD::PUBREC, 2);
    netPacketId = boost::asio::detail::socket_ops::host_to_network_long(packetId);
    std::array<asio::const_buffer,2> buf = {
        buffer(packet.data(),packet.length()),
        buffer(&netPacketId,sizeof(uint16_t))
    };
    co_await async_write(*this->sslSocket_, buf, asio::use_awaitable);
    spdlog::info("PUBREC: send packetId = [X'{:04X}']",packetId);
}

awaitable<void> MqttSession::handleSubscribe() {
    uint16_t packetId;
    string tmTopic;
    uint8_t tmpQos;
    list<std::pair<string, uint8_t>> subTopicList;
    string subackPayload;
    packetId =  recvBuf_.readInt16();
    if(packetId == 0) {
        throw MqttException((int)MQTT_RC_CODE::ERR_MALFORMED_PACKET,fmt::format("packetid is invalid packetid:{}",packetId));
    }
    if(recvBuf_.readableBytes()  == 0) {
        throw MqttException((int)MQTT_RC_CODE::ERR_MALFORMED_PACKET,"no subscribe list");
    }

    while(recvBuf_.readableBytes() > 0 ) {
        uint16_t topicLen =  recvBuf_.readInt16();
        checkReadableBytes(topicLen, fmt::format("topicLen is invalid :{}",topicLen));
        tmTopic =  recvBuf_.retrieveAsString(topicLen);
        
    }

}

awaitable<void> MqttSession::handleUnsubscribe() {

}

MqttSession::~MqttSession() { SPDLOG_INFO("~MqSession() remote_address: {}", address_); }
