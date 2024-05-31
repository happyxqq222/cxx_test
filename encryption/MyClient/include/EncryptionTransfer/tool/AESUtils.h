#pragma once

#include "SslTypes.h"
#include <json/config.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string>
#include <string_view>

class AESUtils {
public:
   AESUtils(std::string_view secretKey,std::string_view iv);
   AESUtils(std::string&& secretKey, std::string&& iv);
   /**
    * @brief 随机生成向量和密钥
    * 
    * @param keyLen 密钥的长度 
    * @param ivLen  向量的长度
    * @return std::tuple<std::string,std::string> 1.密钥文本 2.向量文本
    */
   static std::string generateRandomStr(int len);
   std::string encrypt(std::string_view plainText);
   std::string decrypt(std::string_view cipherText);
   std::string& getIv();
   std::string& getSecretKey();
private:
   std::string iv_ ;
   std::string secretKey_;
};