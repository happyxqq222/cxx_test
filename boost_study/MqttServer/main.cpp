#include <cmath>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>

#include "ByteBuffer.h"
#include "MqttAcl.h"
#include "MqttConfig.h"
#include "MqttException.h"
#include "MqttLogger.h"
#include "MqttServer.h"
#include "spdlog/spdlog.h"

using namespace std;
using namespace boost::asio;

void testStacktrace() { /* throw MqttException("error mqtt"); */
}



void enable_openssl_logging() {
    // 初始化OpenSSL库，并加载错误字符串
    OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);

    // 设置OpenSSL调试日志回调函数
    SSL_CTX* ctx = SSL_CTX_new(TLS_method());
    SSL_CTX_set_info_callback(ctx, [](const SSL* ssl, int where, int ret) {
        const char* str;
        int w = where & ~SSL_ST_MASK;

        if (w & SSL_ST_CONNECT) str = "SSL_connect";
        else if (w & SSL_ST_ACCEPT) str = "SSL_accept";
        else str = "undefined";

        if (where & SSL_CB_LOOP) {
            spdlog::debug("{}: {} ({})", str, SSL_state_string_long(ssl), SSL_state_string(ssl));
        } else if (where & SSL_CB_ALERT) {
            str = (where & SSL_CB_READ) ? "read" : "write";
            spdlog::debug("SSL3 alert {}: {} ({})", str, SSL_alert_type_string_long(ret), SSL_alert_desc_string_long(ret));
        } else if (where & SSL_CB_EXIT) {
            if (ret == 0) {
                spdlog::debug("{}: failed in {} ({})", str, SSL_state_string_long(ssl), SSL_state_string(ssl));
            } else if (ret < 0) {
                spdlog::debug("{}: error in {} ({})", str, SSL_state_string_long(ssl), SSL_state_string(ssl));
            }
        }
    });

    SSL_CTX_free(ctx); // 释放临时上下文
}



int main() {
/*     thread t([](){
        while(true){
            this_thread::sleep_for(1s);
        }
    }); */
    MqttConfig* mqttConfig = MqttConfig::getInstance();
    mqttConfig->parse("config.yml");

    MqttLogger::getInstance()->init(MqttConfig::getInstance()->name(),
                                    MqttConfig::getInstance()->max_rotate_size(),
                                    MqttConfig::getInstance()->max_rotate_count());
    std::shared_ptr<spdlog::logger> logger = spdlog::default_logger();
    try {
        MqttServer mqttServer(1883);
        mqttServer.start();
    } catch (std::exception& e) {
        SPDLOG_INFO("exception:{}", e.what());
    }

    // SPDLOG_INFO("test ");
    // MqttAcl mqttAcl;
    // mqttAcl.loadAcl("acl_rules.yml");
    return 0;
}