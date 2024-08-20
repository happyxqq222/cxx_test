#pragma once

#include <string>
#include <string_view>
#include <boost/core/noncopyable.hpp>
#include <vector>
#include "MprpcConfig.h"

class MprpcApplication : boost::noncopyable {
private:
    MprpcApplication() = default;

public:
    static void init(const std::string& fileName);
    static MprpcApplication& getInstance() {
        static MprpcApplication instance;
        return instance;
    }
    static MprpcConfig& getConfig();

private:
    static MprpcConfig config_;
};