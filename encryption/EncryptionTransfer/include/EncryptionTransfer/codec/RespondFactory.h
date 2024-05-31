//
// Created by Administrator on 2024-05-09.
//

#pragma once

#include "ResponseCodec.h"
#include "CodecFactory.h"

class RespondFactory : public CodecFactory{
public:
    explicit RespondFactory(std::string_view enc);
    explicit RespondFactory(const ResponseInfo& info);
    std::shared_ptr<Codec> createCodec() override;
    ~RespondFactory() override = default;

private:
    bool flag_;
    std::string encStr_;
    ResponseInfo info_;
};


