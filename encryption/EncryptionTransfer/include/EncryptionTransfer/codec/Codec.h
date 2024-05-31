//
// Created by Administrator on 2024-05-09.
//

#pragma once

#include <string>
#include <any>

class Codec {
public:
    Codec();
    virtual std::string encodeMsg();
    virtual std::any decodeMsg();
    virtual ~Codec();
};


