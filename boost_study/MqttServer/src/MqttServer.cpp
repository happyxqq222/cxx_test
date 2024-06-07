//
// Created by xqq on 24-6-6.
//

#include "MqttServer.h"

#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/verify_mode.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <csignal>
#include <exception>
#include <functional>
#include <memory>
#include <string>

#include "MqttCommon.h"
#include "MqttConfig.h"
#include "MqttSession.h"
#include "spdlog/spdlog.h"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

MqttServer::MqttServer(const std::string& host, uint16_t port)
    : ioc_(1), signals_(ioc_), acceptor_(ioc_){}

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

void MqttServer::run() noexcept {
    try{
        init();
        SPDLOG_INFO("Mqtt Server start");
        SPDLOG_INFO("Mqtt Server Listening on {}",convert::format_address(listenEndpoint_));
        SPDLOG_INFO("Mqtt Server Listening Address type :{}",listenEndpoint_.address().is_v4() ?"IPV4" : "IPV6");
        //支持TLS1.2 和 TLS1.3版本
        if(MqttConfig::getInstance()->version() == MqttConfig::VERSION::TLSv13) {
            sslContext_ = make_unique<ssl::context>(ssl::context::tlsv13);
            SPDLOG_INFO("Mqtt Server SSL/TLS Version is TLSv1.3");
        } else {
            sslContext_ = make_unique<ssl::context>(ssl::context::tlsv12);
            SPDLOG_INFO("Mqtt Server SSL/TLS Version is TLSv1.2");
        }
        auto sslOptions = ssl::context::default_workarounds | ssl::context::no_sslv2 |
                          ssl::context::no_sslv3 | ssl::context::no_tlsv1 | ssl::context::no_tlsv1_1;

        //dh文件用于密钥交换算法                  
        string dhparamFile = MqttConfig::getInstance()->dhparam();
        if(!dhparamFile.empty()) {
            sslOptions |= ssl::context::single_dh_use;
            sslContext_->use_tmp_dh_file(dhparamFile);
        }
        sslContext_->set_options(sslOptions);
        auto mode = asio::ssl::verify_none;
        if(MqttConfig::getInstance()->verify_mode() == MqttConfig::SSL_VERIFY::PEER) {

        }
    }catch(const std::exception& e) {
        SPDLOG_ERROR("Mqtt Server Failed to start : ERR_MSG = [{}]",e.what());
    }
}

void MqttServer::stop() { ioc_.stop(); }

asio::awaitable<void> MqttServer::handle_accept() {
    try {
        for (;;) {
            ssl::stream<tcp::socket> sslSocket(ioc_, *sslContext_);
            co_await acceptor_.async_accept(sslSocket.next_layer(), use_awaitable);
            std::make_shared<MqttSession>();
        }
    } catch (const std::exception& e) {
        SPDLOG_INFO("run error : {}", e.what());
    }
}

MqttServer::~MqttServer() {}