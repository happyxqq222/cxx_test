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



class MqttSession;
class MqttBroker;

class MqttServer {

public:
   MqttServer(uint16_t port);
   ~MqttServer();

   void start() noexcept;

private:
   void init();

   void stop();
   
   void startAccept();
   void handleAccept(std::shared_ptr<MqttSession> newSession,const boost::system::error_code error);
   //接受acceptor的ioc
   boost::asio::io_context mainIoc_; 
   boost::asio::signal_set signals_;
   boost::asio::ip::tcp::acceptor acceptor_;
   boost::asio::ip::tcp::endpoint listenEndpoint_;
   std::unique_ptr<boost::asio::ssl::context> sslContext_;
   std::unique_ptr<MqttBroker> mqttBroker;
   bool isClose = false;
};


#endif //MQTTSERVER_MQTTSERVER_H
