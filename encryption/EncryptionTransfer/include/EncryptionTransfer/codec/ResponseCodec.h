//
// Created by Administrator on 2024-05-09.
//
#pragma once

#include "Codec.h"
#include "Message.pb.h"

#include <boost/core/noncopyable.hpp>

struct ResponseInfo {
    int cmdType;
    int status;
    int secKeyId;
    std::string clientID;
    int timestamp;
    std::string serverID;
    std::string data;
};

class ResponseCodec : public Codec, boost::noncopyable {

public:
    ResponseCodec();

    //构造出的对象用于解码的场景
    explicit ResponseCodec(std::string_view encStr);

    //构造出的对象用于编码场景
    explicit ResponseCodec(const ResponseInfo &info);

    // init函数给空构造准备的
    void initMessage(const std::string &encStr);

    void initMessage(const ResponseInfo &info);

    std::string encodeMsg() override;

    std::any decodeMsg() override;

    ~ResponseCodec() override;


private:
    std::string encStr_;
    ResponseMsg msg_;
};


