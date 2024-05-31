#include "EncryptionTransfer/tool/AESUtils.h"
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <muduo/base/Logging.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/types.h>
#include <string>
#include <string_view>
#include <random>
#include "EncryptionTransfer/tool/SslTypes.h"

using namespace std;

AESUtils::AESUtils(string_view secretKey, string_view iv) : iv_(iv), secretKey_(secretKey) {}

AESUtils::AESUtils(std::string&& secretKey, std::string&& iv) :iv_(std::move(iv)),secretKey_(std::move(secretKey)) {

};

std::string AESUtils::encrypt(string_view plainText) {
    int ret = 0;
    int cipherTextLen = 0;
    int len = 0;
    string cipherText(plainText.size() + EVP_MAX_BLOCK_LENGTH, 0);
    //创建上下文
    std::unique_ptr<EVP_CIPHER_CTX> ctx = unique_ptr<EVP_CIPHER_CTX>(EVP_CIPHER_CTX_new());
    if (!ctx) {
        sslErrorHandle("EVP_CIPHER_CTX_new error");
    }
    //初始化加密操作
    ret = EVP_EncryptInit_ex(ctx.get(),
                             EVP_aes_128_cbc(),
                             nullptr,
                             (unsigned char*)secretKey_.data(),
                             (unsigned char*)iv_.data());
    if (ret != 1) {
        sslErrorHandle("EVP_EncryptInit_ex error");
    }

    //执行加密操作
    ret = EVP_EncryptUpdate(ctx.get(),
                            (unsigned char*)cipherText.data(),
                            &len,
                            (unsigned char*)plainText.data(),
                            plainText.size());
    if (ret != 1) {
        sslErrorHandle("EVP_EncryptUpdate error");
    }
    cipherTextLen = len;
    //结束加密操作
    ret = EVP_EncryptFinal(ctx.get(), (unsigned char*)cipherText.data() + cipherTextLen, &len);
    if (ret != 1) {
        sslErrorHandle("EVP_EncryptFinal error");
    }
    cipherTextLen = cipherTextLen + len;
    return string(cipherText.data(), cipherText.data() + cipherTextLen);
}

std::string AESUtils::decrypt(std::string_view cipherText) {
    int ret = 0;
    int plainTextLen = 0;
    int len = 0;
    string plainText(cipherText.length() + EVP_MAX_BLOCK_LENGTH, 0);
    //创建上下文
    std::unique_ptr<EVP_CIPHER_CTX> ctx = unique_ptr<EVP_CIPHER_CTX>(EVP_CIPHER_CTX_new());
    if (!ctx) {
        sslErrorHandle("EVP_CIPHER_CTX_new error");
    }
    //初始化解密操作 使用AES-128-cbc模式
    ret = EVP_DecryptInit_ex(ctx.get(),
                             EVP_aes_128_cbc(),
                             nullptr,
                             (unsigned char*)secretKey_.data(),
                             (unsigned char*)iv_.data());
    if (ret != 1) {
        sslErrorHandle("EVP_DecryptInit_ex error");
    }
    //执行解密操作
    ret = EVP_DecryptUpdate(ctx.get(),
                            (unsigned char*)plainText.data(),
                            &len,
                            (unsigned char*)cipherText.data(),
                            cipherText.size());
    if (ret != 1) {
        sslErrorHandle("EVP_DecryptUpdate error");
    }
    plainTextLen = len;
    //结束解密操作
    ret = EVP_DecryptFinal_ex(ctx.get(), (unsigned char*)plainText.data() + len, &len);
    if (ret != 1) {
        sslErrorHandle("EVP_DecryptFinal_ex");
    }
    plainTextLen = plainTextLen + len;
    return string(plainText.data(), plainText.data() + plainTextLen);
}

std::string AESUtils::generateRandomStr(int len) {
    string key(len, 0);
    /*     int ret = RAND_bytes((unsigned char*)key.data(), keyLen);
    if(ret != 1) {
        sslErrorHandle("RAND_bytes error");
    }
    ret = RAND_bytes((unsigned char*)iv.data(), ivLen);
    if(ret != 1) {
        sslErrorHandle("RAND_bytes error");
    } */
    std::random_device rd;
    mt19937 gen(rd());
    string_view specialCharacter = "~!@#$%^&*()_+{}|";
    uniform_int_distribution<> random26(0,25);
    uniform_int_distribution<> random10(0,9);
    uniform_int_distribution<> randomSpecial(0,specialCharacter.size()-1);

    int flag;
    for (int i = 0; i < len; i++) {
        flag = rand() % 4;
        switch (flag) {
            case 0:  //a-z
              key.data()[i] = 'a' + random26(gen); 
              break;
            case 1: //A-Z
              key.data()[i] = 'A' + random26(gen); 
              break;
            case 2: //0-9
              key.data()[i]  = '0' + random10(gen); 
              break;
            case 3: //特殊字符
              key.data()[i] =  specialCharacter[randomSpecial(gen)];
              break;
        }
    }
    return key;
}

std::string& AESUtils::getIv() {
    return iv_;
}
std::string& AESUtils::getSecretKey() {
    return secretKey_;
}