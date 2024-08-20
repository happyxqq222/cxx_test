#include "MprpcConfig.h"
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <fstream>
#include <ios>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <iostream>

using namespace std;

void MprpcConfig::loadConfigFile(std::string_view fileName) {
    Json::Reader reader;
    Json::Value root;
    std::ifstream ifs(fileName.data(), std::ios_base::in);
    shared_ptr<ifstream> ifsPtr = shared_ptr<ifstream>(&ifs,[](ifstream* p){
        p->close();
    });
    reader.parse(ifs,root);
    string serverIp = root["serverIp"].asString();
    string serverPort = root["serverPort"].asString();
    string zookeeperIp = root["zookeeperIp"].asString();
    string zookeeperPort = root["zookeeperPort"].asString();
    configMap.insert({"serverIp",std::move(serverIp)});
    configMap.insert({"serverPort",std::move(serverPort)});
    configMap.insert({"zookeeperIp",std::move(zookeeperIp)});
    configMap.insert({"zookeeperPort",std::move(zookeeperPort)});
}

std::optional<std::reference_wrapper<std::string>> MprpcConfig::getConfig(const std::string& key) {
    auto it = configMap.find(key);
    if(it != configMap.end()) {
        return it->second ;
    } else {
        return nullopt;
    }
}