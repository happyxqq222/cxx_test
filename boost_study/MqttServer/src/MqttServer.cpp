//
// Created by xqq on 24-6-6.
//

#include "MqttServer.h"
#include <pthread.h>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/context_base.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/verify_mode.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <csignal>
#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <utility>

#include "MqttCommon.h"
#include "MqttConfig.h"
#include "MqttIoServicePool.h"
#include "MqttSession.h"
#include "MqttBroker.h"
#include "spdlog/spdlog.h"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

MqttServer::MqttServer(uint16_t port)
    : mainIoc_()
    , signals_(mainIoc_)
    , acceptor_(mainIoc_)
    , listenEndpoint_(tcp::v4(),port)
    , mqttBroker(make_unique<MqttBroker>()) {}

void MqttServer::init() {
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
    signals_.add(SIGQUIT);
    signals_.async_wait(bind(&MqttServer::stop, this));
    acceptor_.open(listenEndpoint_.protocol());
    acceptor_.set_option(tcp::acceptor::reuse_address(true));
    acceptor_.bind(listenEndpoint_);
    acceptor_.listen();
}

void MqttServer::start() noexcept {
    // try {
        init();
        SPDLOG_INFO("Mqtt Server start");
        SPDLOG_INFO("Mqtt Server Listening on {}", convert::format_address(listenEndpoint_));
        SPDLOG_INFO("Mqtt Server Listening Address type :{}",
                    listenEndpoint_.address().is_v4() ? "IPV4" : "IPV6");
        //支持TLS1.2 和 TLS1.3版本
        if (MqttConfig::getInstance()->version() == MqttConfig::VERSION::TLSv13) {
            sslContext_ = make_unique<ssl::context>(ssl::context::tlsv13_server);
            SPDLOG_INFO("Mqtt Server SSL/TLS Version is TLSv1.3");
        } else {
            sslContext_ = make_unique<ssl::context>(ssl::context::tlsv12_server);
            SPDLOG_INFO("Mqtt Server SSL/TLS Version is TLSv1.2");
        }
        auto sslOptions = ssl::context::default_workarounds | ssl::context::no_sslv2 |
                          ssl::context::no_sslv3 | ssl::context::no_tlsv1 |
                          ssl::context::no_tlsv1_1 ;

        sslContext_->set_options(sslOptions);

        //设置证书校验模式
        auto mode = asio::ssl::verify_none;
        // if (MqttConfig::getInstance()->verify_mode() == MqttConfig::SSL_VERIFY::PEER) {
        //     mode = mode | ssl::verify_fail_if_no_peer_cert;
        //     if (MqttConfig::getInstance()->fail_if_no_peer_cert()) {
        //         mode |= ssl::verify_fail_if_no_peer_cert;
        //     }
        //     //双向认证必须提供证书
        //     sslContext_->load_verify_file(MqttConfig::getInstance()->cacertfile());
        //     SPDLOG_INFO("Mqtt Server SSL/TLS Verify Mode is verify_peer");
        // } else {
        //     SPDLOG_INFO("Mqtt Server SSL/TLS Verify Mode is Verify_none");
        // }
        sslContext_->set_verify_mode(mode);

        string password = MqttConfig::getInstance()->password();
        if (!password.empty()) {
            sslContext_->set_password_callback(
            [pw = std::move(password)](size_t, ssl::context_base::password_purpose) {
                return pw;
            });
        }

        std::string serverCrt = MqttConfig::getInstance()->certfile();  //公钥文件
        std::string serverKey = MqttConfig::getInstance()->keyfile();   //私钥文件
        SPDLOG_INFO("Mqtt Server certfile is {}",serverCrt);
        SPDLOG_INFO("Mqtt Server private key file is {}",serverKey);

        sslContext_->use_certificate_chain_file(serverCrt);
        sslContext_->use_private_key_file(serverKey,ssl::context::pem);
        startAccept();
        SPDLOG_INFO("main threadid:{}",pthread_self());
        mainIoc_.run();
    // } catch (const std::exception& e) {
    //     SPDLOG_ERROR("Mqtt Server Failed to start : ERR_MSG = [{}]", e.what());
    // }
}

void MqttServer::stop() { 
    mainIoc_.stop();
    MqttIoServicePool::getInstance().stop();
}

void MqttServer::startAccept() {
    auto& nextIoc = MqttIoServicePool::getInstance().getIoContext();
    unique_ptr<ssl::stream<tcp::socket>> sslSocket = make_unique<ssl::stream<tcp::socket>>(nextIoc,*sslContext_);
    std::shared_ptr<MqttSession> mqttSession = make_shared<MqttSession>(sslSocket,nextIoc,mqttBroker.get(),isClose);
    acceptor_.async_accept(mqttSession->getSslSocket().next_layer(),std::bind(&MqttServer::handleAccept,this,mqttSession,std::placeholders::_1));
}

void MqttServer::handleAccept(std::shared_ptr<MqttSession> newSession,const boost::system::error_code error) {
    SPDLOG_INFO("accept threadid:{}",pthread_self());
    if(!error && newSession != nullptr) {
        newSession->start();
    } else {
        SPDLOG_ERROR("handleAccept error:{}",error.what());
    }
    startAccept();
}



MqttServer::~MqttServer() {
    SPDLOG_INFO("~MqttServer"); 
}