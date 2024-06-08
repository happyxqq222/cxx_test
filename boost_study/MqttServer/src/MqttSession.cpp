//
// Created by xqq on 24-6-6.
//

#include "MqttSession.h"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <boost/asio/ssl/stream_base.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/detail/frame_unwind.ipp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "MqttBroker.h"
#include "MqttCommon.h"
#include "MqttConfig.h"
#include "MqttException.h"
#include "spdlog/spdlog.h"

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace boost;
using namespace boost::asio;
using namespace boost::stacktrace;
using namespace boost::system;

MqttSession::MqttSession(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> sslSocket,
                         io_context& curIoc, MqttBroker& broker)
    : sslSocket_(std::move(sslSocket)),
      curIoc_(curIoc),
      broker_(broker),
      condTimer_(sslSocket_.get_executor()),
      keepAliveTimer_(sslSocket.get_executor()),
      checkTimer_(sslSocket_.get_executor()),
      completeConnect_(false),
      rc_(MQTT_RC_CODE::ERR_SUCCESS),
      buf_(1024, 0),
      command_(0),
      pos_(0) {
    this->condTimer_.expires_at(steady_clock::time_point::max());
    this->keepAliveTimer_.expires_at(steady_clock::time_point::max());
    this->checkTimer_.expires_at(steady_clock::time_point::max());
}

void MqttSession::start() {
    co_spawn(
        curIoc_,
        [self = shared_from_this()]() -> awaitable<void> { return self->handleHandshake(); },
        detached);
}

boost::asio::awaitable<void> MqttSession::handleHandshake() {
    try {
        //这地方会等握手返回然后再执行handlSession.同时协程上下文会调度执行别协程代码
        co_await sslSocket_.async_handshake(ssl::stream_base::server, use_awaitable);
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
    asio::co_spawn(
        curIoc_, [self = shared_from_this()] { return self->handleKeepAlive(); }, detached);
}

void MqttSession::disconnect() {
    system::error_code ignoredEc;
    this->sslSocket_.shutdown(ignoredEc);
    this->sslSocket_.next_layer().close(ignoredEc);
    this->condTimer_.cancel(ignoredEc);
    this->keepAliveTimer_.cancel(ignoredEc);
    this->checkTimer_.cancel(ignoredEc);
}

boost::asio::awaitable<void> MqttSession::handleKeepAlive() {
    auto checkDuration = seconds(MqttConfig::getInstance()->check_timeout_duration());
    while (this->sslSocket_.next_layer().is_open()) {
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
    while (isClose_) {
        try {
            size_t n = co_await sslSocket_.async_read_some(buffer(buf_.data(), buf_.size()),
                                                           use_awaitable);
            if (n == 0) {
                throw MqttException((int)MQTT_RC_CODE::ERR_PEER_CLOSE, "receive peer closed");
            } else {
                recvBuf_.append(buf_.data(), n);
                if (recvBuf_.readableBytes() > 0) {
                    if (parseState_ == ParseState::NONE) {
                        readCommand();
                    } else if (parseState_ == ParseState::COMMAND) {
                        readRemainingLength();
                    } else if (parseState_ == ParseState::REMAINING_LENGTH) {
                        if (recvBuf_.readableBytes() >= 1 + remainingCount_ + remainingLength_) {
                            //先把buf里的COMMAND和剩余长度读取掉
                            recvBuf_.retrieve(1 + remainingCount_);
                            co_await handleRemainingBody();
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

void MqttSession::checkReadableBytes(uint32_t readableBytes,std::string_view errorMsg) {
    if(recvBuf_.readableBytes() < readableBytes) {
        SPDLOG_ERROR("expected readableBytes:{},really readableBytes:{}",readableBytes,recvBuf_.readableBytes());
        throw MqttException((int)MQTT_RC_CODE::ERR_REMAINING_LENGTH,errorMsg);
    }
}

void MqttSession::readCommand() {
    this->command_ = recvBuf_.peekInt8();
    SPDLOG_DEBUG("command = [X'{:02X}']", static_cast<uint16_t>(command));
    checkCommand();
    parseState_ = ParseState::COMMAND;
}

void MqttSession::checkCommand() {
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
        vector<uint8_t> remainingLength(4, 0);
        //存储长度的字节数
        int remainingCount = 0;
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
            int len = (int)*(reinterpret_cast<int*>(remainingLength.data()));
            this->remainingCount_ = remainingCount;
            this->remainingLength_ = len;
            SPDLOG_DEBUG("remaining length = [{}]", len);
            checkRemainingLength();
            parseState_ = ParseState::REMAINING_LENGTH;
            return;
        } else {
            this->remainingCount_ = 0;
            this->remainingLength_ = 0;
            SPDLOG_DEBUG("read remaining length error");
            return;
        }
    }
}

void MqttSession::checkRemainingLength() {
    SPDLOG_ERROR("检测报文长度");
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

awaitable<void> MqttSession::handleRemainingBody() {
    switch (command_ & 0xF0) {
        case MQTT_CMD::CONNECT:
          co_await handleConnect();
        break; 
    }

}

awaitable<void> MqttSession::handleConnect() {
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
    string username,password;
    uint16_t keepAlive;
    string clientId;

    //读取可变报头
    //1.读取协议名称长度
    checkReadableBytes(2, "[conn] read protocolLen remaining length is error");
    uint16_t protocolNameLen = recvBuf_.readInt16();
    SPDLOG_DEBUG("CONNECT: read protocol name length :{}",protocolNameLen);
    if(protocolNameLen != 4) {
        SPDLOG_ERROR("[conn] protocolName length must be 4 but is {}",protocolNameLen);
        throw MqttException((int)MQTT_RC_CODE::ERR_PROTOCOL,"[conn] protocolName length must be 4");
    }

    //2.读取协议名称
    checkReadableBytes(protocolNameLen, "[conn] read protocolName remaining length is error");
    string protocolName = recvBuf_.retrieveAsString(protocolNameLen);
    SPDLOG_DEBUG("CONNECT: read protocol name:{}",protocolName);

    if(protocolName != "MQTT") {
        SPDLOG_ERROR("protocol_name is error = {}",protocolName);
        throw MqttException((int)MQTT_RC_CODE::ERR_PROTOCOL,"[conn] protocol name must be MQTT");
    }

    //3.读取协议版本
    checkReadableBytes(1, "[conn] read protocolVersion remaining length is error");
    uint8_t protocolVersion = recvBuf_.readInt8();
    SPDLOG_DEBUG("protocol version:{}",protocolVersion);
    //MQTT 3.1.1
    if(protocolVersion != 0x04) {
        SPDLOG_ERROR("protocol version is error = {}",protocolVersion);
        //协议名称错误 发送connAck
        co_await sendConnack(0x00,MQTT_CONNACK::REFUSED_PROTOCOL_VERSION);
        throw MqttException((int)MQTT_RC_CODE::ERR_PROTOCOL,"[conn] protocol version is error");
    }

    //4.读取标志位
    checkReadableBytes(1, "[conn] read connect flag remaining length is error");
    uint8_t connectFlag = recvBuf_.readInt8();
    SPDLOG_INFO("connect flag:{}",connectFlag);
    cleanSession = (connectFlag & 0x02) >> 1;
    willFlag = connectFlag & 0x04;
    willTopic.qos = (connectFlag & 0x18) >> 3;
    willTopic.retain = ((connectFlag & 0x20) == 0x20);
    passwordFlag = connectFlag & 0x40;
    usernameFlag = connectFlag & 0x80;
    
    //遗嘱保留级别为3 无效
    if(willTopic.qos == 3) {
        SPDLOG_ERROR("will qos is invalid qos: {}",(uint8_t)willTopic.qos);
        throw MqttException((int)MQTT_RC_CODE::ERR_PROTOCOL,"[conn] will qos is invalid");
    }

    // 如果禁用遗嘱时(willFlag = false) 遗嘱qos必须为0而且遗嘱消息不能设置为保留
    if(willFlag == false && (willTopic.qos != 0 || willTopic.retain)) {
        SPDLOG_ERROR("if testament is disable qos shuould be 0 and retain is disable ,qos:{},retain:{}",(uint8_t)willTopic.qos,(uint8_t)willTopic.retain);
        throw MqttException((int)MQTT_RC_CODE::ERR_PROTOCOL,"[conn] if testament is disable qos shuould be 0 and retain is disable");
    }

    //4.读取keepAlive
    checkReadableBytes(2, "[conn] read keepalive  remaining length is error");
    keepAlive = recvBuf_.readInt16();
    SPDLOG_DEBUG("[conn] keepalive:{}",keepAlive);

    //5.读取客户端标识长度
    checkReadableBytes(2, "[conn] read clientid length  remaining length is error");
    uint16_t clientIdLen = recvBuf_.readInt16();

    //mqtt 3.1.1 如果客户端标识符长度为 0，清理会话标志必须设置为 1（True）
    if(clientIdLen == 0 && !cleanSession) {
        SPDLOG_ERROR("when clientIdLen equals 0 the clientSession must be set true");
        co_await sendConnack(0x00, MQTT_CONNACK::REFUSED_IDENTIFIER_REJECTED);
        throw MqttException((int)MQTT_RC_CODE::ERR_PROTOCOL,"[conn] when clientIdLen equals 0 the clientSession must be set true");
    }

    //6.读取客户端id
    checkReadableBytes(clientIdLen, "[conn] read clientid  remaining length is error");
    clientId = recvBuf_.retrieveAsString(clientIdLen);
     
    // clientid_ 前缀用于自动生成的 client_id, 客户端生成的不能带有 clientid_ 前缀
    if(clientId.starts_with("clientid_")) {
        SPDLOG_ERROR("clientid can't start with clientid_");
        co_await sendConnack(0x00, MQTT_CONNACK::REFUSED_IDENTIFIER_REJECTED);
        throw MqttException((int)MQTT_RC_CODE::ERR_BAD_CLIENT_ID,"[conn] clientid can't start with clientid_");
    }

    if(clientId.empty()) {
        //生成clientid
        clientId = "clientid_"+ boost::uuids::to_string(uuidGenerator());
    }

    //读取遗嘱消息
    if(willFlag) {
        //7.读取遗嘱主题长度
        checkReadableBytes(2, "[conn] read topicName len  remaining length is error");
        uint16_t topicNameLen = recvBuf_.readInt16();

        //8.读取遗嘱主题
        checkReadableBytes(topicNameLen, "[conn] read topicName  remaining length is error");
        string topicName = recvBuf_.retrieveAsString(topicNameLen);

        //9.读取遗嘱消息长度
        checkReadableBytes(2, "[conn] read paylod len  remaining length is error");
        uint16_t payloadLen = recvBuf_.readInt16();

        //10.读取遗嘱消息
        checkReadableBytes(payloadLen, "[conn] read paylod  remaining length is error");
        string paylod = recvBuf_.retrieveAsString(payloadLen);
        
        willTopic.topic_name = std::make_shared<const string>(std::move(topicName));
        willTopic.paylod = std::make_shared<const string>(std::move(paylod));
    }

    //11.读取用户名
    if(usernameFlag) {
        checkReadableBytes(2, "[conn] read username len  remaining length is error");
        uint16_t userNameLen = recvBuf_.readInt16();
        checkReadableBytes(userNameLen, "[conn] read username  remaining length is error");
        username = recvBuf_.retrieveAsString(userNameLen);
    }

    //12.读取密码
    if(usernameFlag) {
        checkReadableBytes(2, "[conn] read password len  remaining length is error");
        uint16_t passwordLen = recvBuf_.readInt16();
        checkReadableBytes(passwordLen, "[conn] read password  remaining length is error");
        password = recvBuf_.retrieveAsString(passwordLen);
    }

    //进行密码校验
    if(MqttConfig::getInstance()->auth()) {
        if(!MqttConfig::getInstance()->auth(username,password)) {
            SPDLOG_ERROR("username or password is incorrect username:{},password:{}",username,password);
            co_await sendConnack(0x00,MQTT_CONNACK::REFUSED_BAD_USERNAME_PASSWORD);
            throw MqttException((int)MQTT_RC_CODE::ERR_BAD_USERNAME_PASSWORD,"[conn] username or password is incorrect");
        }
        this->username_ = username;
    }

    //进行ACL校验
    if(MqttConfig::getInstance()->acl_enable()) {
        mqtt_acl_rule_t rule;
        rule.type = MQTT_ACL_TYPE::IPADDR;
        system::error_code ec;
        rule.object = this->sslSocket_.next_layer().remote_endpoint(ec).address().to_string();
        if(ec) {
            throw MqttException((int)MQTT_RC_CODE::ERR_NO_CONN,"[conn] error connection");
        }
        SPDLOG_DEBUG("remote ip addr : [{}]", rule.object);

        if(!MqttConfig::getInstance()->acl_check(rule)) {
            SPDLOG_ERROR("ip is deny ip:{}",rule.object);
            co_await sendConnack(0x00, MQTT_CONNACK::REFUSED_NOT_AUTHORIZED);
            throw MqttException((int)MQTT_RC_CODE::ERR_REFUSED_NOT_AUTHORIZED,"[conn] ip is deny ip:"+rule.object);
        }

        rule.type = MQTT_ACL_TYPE::CLIENTID;
        rule.object = clientId;

        if(!MqttConfig::getInstance()->acl_check(rule)) {
            SPDLOG_ERROR("clientid is deny clientid:{}",rule.object);
            co_await sendConnack(0x00, MQTT_CONNACK::REFUSED_NOT_AUTHORIZED);
            throw MqttException((int)MQTT_RC_CODE::ERR_REFUSED_NOT_AUTHORIZED,"[conn] clientit is deny clientid:"+rule.object);
        }

        this->clientId_ = clientId;
        this->sessionState_.cleanSession_ = cleanSession;
        this->sessionState_.keepAlive_ = keepAlive;
        this->sessionState_.willTopic = willTopic;

        //加入broker
        sessionPresent = this->broker_.joinOrUpdate(shared_from_this());

        //发送CONNACK响应
        co_await sendConnack(sessionPresent, MQTT_CONNACK::ACCEPTED);

        //connect完成标志设置
        this->completeConnect_ = true;
        //添加自动订阅
        addSubscribe(MqttConfig::getInstance()->auto_subscribe_list());

        //开启协程用于处理需要当前会话转发的主题
        asio::co_spawn(curIoc_,[self=shared_from_this()]{
            return self->handleInflightingPackets();
        },detached);

        //开启协程用于处理需要等待一段时间的操作
        co_spawn(curIoc_,[self=shared_from_this()]{
            return self->handleWaitingMapPackets();
        },detached);
        
        SPDLOG_DEBUG("success to handle 'CONNECT', clientid=[{}] cleanSession=[{}]",this->clientId_,this->sessionState_.cleanSession_);
        

    } 









}

std::string& MqttSession::getSessionId() { return this->clientId_; }

void MqttSession::moveSessionState(std::shared_ptr<MqttSession> oldSession) {
    oldSession->disconnect();
}

awaitable<void> MqttSession::sendConnack(uint8_t ack, uint8_t reasonCode) {
    co_return;
}

MqttSession::~MqttSession() {}
