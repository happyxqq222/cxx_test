#pragma once

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
class MprpcConfig {

public:
   void loadConfigFile(std::string_view fileName);
   std::optional<std::reference_wrapper<std::string>> getConfig(const std::string& key);
private:
   std::unordered_map<std::string, std::string> configMap;
};