//
// Created by Administrator on 2024-05-09.
//

#include "EncryptionTransfer/codec/ResponseCodec.h"

ResponseCodec::ResponseCodec(std::string_view encStr) : encStr_(encStr) {

}

ResponseCodec::ResponseCodec(const ResponseInfo &info){
    msg_.set_cmdtype(info.cmdType);
    msg_.set_status(info.status);
    msg_.set_seckeyid(info.secKeyId);
    msg_.set_clientid(info.clientID);
    msg_.set_timestamp(info.timestamp);
    msg_.set_serverid(info.serverID);
    msg_.set_data(info.data);
}

void ResponseCodec::initMessage(const std::string &encStr) {
    this->encStr_ = encStr;
}

ResponseCodec::ResponseCodec() = default;

void ResponseCodec::initMessage(const ResponseInfo &info) {

}

std::string ResponseCodec::encodeMsg() {
    return std::move(msg_.SerializeAsString());
}

std::any ResponseCodec::decodeMsg() {
    return Codec::decodeMsg();
}

ResponseCodec::~ResponseCodec() = default;
