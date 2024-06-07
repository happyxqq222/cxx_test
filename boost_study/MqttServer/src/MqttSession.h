//
// Created by xqq on 24-6-6.
//

#ifndef MQTTSERVER_MQTTSESSION_H
#define MQTTSERVER_MQTTSESSION_H

#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ssl/stream.hpp>

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

class MqttSession {

private:
   ssl::stream<tcp::socket> socket;
};


#endif //MQTTSERVER_MQTTSESSION_H
