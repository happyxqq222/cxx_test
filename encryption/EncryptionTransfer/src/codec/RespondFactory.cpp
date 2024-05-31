//
// Created by Administrator on 2024-05-09.
//

#include "EncryptionTransfer/codec/RespondFactory.h"

RespondFactory::RespondFactory(std::string_view enc) : flag_(false), encStr_(enc){

}

RespondFactory::RespondFactory(const ResponseInfo &info) : flag_(true), info_(std::move_if_noexcept(const_cast<ResponseInfo&>(info))){
}

std::shared_ptr<Codec> RespondFactory::createCodec() {
    if(flag_){
        return std::make_shared<ResponseCodec>(info_);
    }else{
        return std::make_shared<ResponseCodec>(encStr_);
    }
}
