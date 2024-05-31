//
// Created by Administrator on 2024-05-09.
//

#include "EncryptionTransfer/codec/RequestCodec.h"

using namespace std;

RequestCodec::RequestCodec() = default;

RequestCodec::RequestCodec(const std::string &encStr) {
    initMessage(encStr);
}

RequestCodec::RequestCodec(const RequestInfo &info) {
    initMessage(info);
}

void RequestCodec::initMessage(const std::string &encStr) {
    this->encStr_ = encStr;
}

void RequestCodec::initMessage(const RequestInfo &info) {
    msg_.set_cmdtype(info.cmdType);
    msg_.set_clientid(info.clientId);
    msg_.set_timestamp(info.timestamp);
    msg_.set_serverid(info.serverId);
    msg_.set_sign(info.sign);
    msg_.set_data(info.data);
}

std::string RequestCodec::encodeMsg() {
    return std::move(msg_.SerializeAsString());
}

std::any RequestCodec::decodeMsg() {
    msg_.ParseFromString(encStr_);
    return std::ref(msg_);
}

RequestCodec::~RequestCodec() = default;

