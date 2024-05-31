//
// Created by Administrator on 2024-05-09.
//
#pragma once

#include "Codec.h"
#include "CodecFactory.h"
#include "RequestCodec.h"

#include <string>

class RequestFactory : public CodecFactory{

public:
    explicit RequestFactory(std::string_view enc);
    explicit RequestFactory(const RequestInfo& info);
    std::shared_ptr<Codec> createCodec() override;
    ~RequestFactory() override = default;

private:
    bool flag_;
    std::string encStr_;
    RequestInfo info_;
};


