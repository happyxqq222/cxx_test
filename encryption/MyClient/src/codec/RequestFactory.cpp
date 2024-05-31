//
// Created by Administrator on 2024-05-09.
//

#include "EncryptionTransfer/codec/RequestFactory.h"
#include "EncryptionTransfer/codec/RequestCodec.h"

RequestFactory::RequestFactory(std::string_view  enc) : CodecFactory(),flag_(false),encStr_(enc){
}

RequestFactory::RequestFactory(const RequestInfo &info) : CodecFactory(),flag_(true),info_(info){

}

std::shared_ptr<Codec> RequestFactory::createCodec() {
    if(flag_){
        return std::make_shared<RequestCodec>(info_);
    }else{
        return std::make_shared<RequestCodec>(encStr_);
    }
}
