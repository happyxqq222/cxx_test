//
// Created by xqq on 24-6-6.
//

#ifndef MQTTSERVER_MQTTSERVER_H
#define MQTTSERVER_MQTTSERVER_H

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstdint>
#include <memory>
#include <boost/asio.hpp>
#include <string>



class MqttSession;

class MqttServer {

public:
   MqttServer(const std::string& host, uint16_t port);
   ~MqttServer();

   void run() noexcept;

private:
   void init();

   void stop();

   boost::asio::awaitable<void> handle_accept();
   boost::asio::io_context ioc_;
   boost::asio::signal_set signals_;
   boost::asio::ip::tcp::acceptor acceptor_;
   boost::asio::ip::tcp::endpoint listenEndpoint_;
   std::unique_ptr<boost::asio::ssl::context> sslContext_;
};


#endif //MQTTSERVER_MQTTSERVER_H
