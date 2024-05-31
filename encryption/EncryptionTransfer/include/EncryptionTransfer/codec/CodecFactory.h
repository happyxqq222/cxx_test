//
// Created by Administrator on 2024-05-09.
//
#pragma once

#include "Codec.h"
#include <memory>


class CodecFactory {

public:
    CodecFactory() = default;

    [[maybe_unused]] virtual std::shared_ptr<Codec> createCodec();
    virtual ~CodecFactory() = default;
};


