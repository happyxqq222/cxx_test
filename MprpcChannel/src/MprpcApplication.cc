#include "MprpcApplication.h"
#include <string>

MprpcConfig MprpcApplication::config_;

void MprpcApplication::init(const std::string& fileName) {
    config_.loadConfigFile(fileName);
}

MprpcConfig& MprpcApplication::getConfig() {
    return config_;
}