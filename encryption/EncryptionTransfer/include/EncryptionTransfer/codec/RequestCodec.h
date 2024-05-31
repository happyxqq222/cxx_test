//
// Created by Administrator on 2024-05-09.
//
#pragma once

#include <json/config.h>
#include "Codec.h"
#include "Message.pb.h"

#include <string>
#include <any>

struct RequestInfo{
    int cmdType;
    std::string clientId;
    int timestamp;
    std::string serverId;
    std::string sign;
    std::string data;
};

class RequestCodec : public Codec {
public:
    RequestCodec();
    //构造出的对象用于解码的场景
    explicit RequestCodec(const std::string& encstr);
    //构造出的对象用于编码场景
    explicit RequestCodec(const RequestInfo& info);
    // init函数给空构造准备的
    void initMessage(const std::string& encStr);
    void initMessage(const RequestInfo& info);
    std::string encodeMsg() override;
    std::any decodeMsg() override;
    ~RequestCodec() override;
private:
    std::string encStr_;
    RequestMsg msg_;
};


