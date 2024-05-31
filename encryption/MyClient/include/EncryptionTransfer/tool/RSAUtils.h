#pragma once

#include "SslTypes.h"
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/types.h>
#include <string>
#include <string_view>

enum class RsaKeyType{
    PRIVATE,
    PUBLIC,
};

class RSAUtil {
public:
    RSAUtil();

    /**
     * @brief 根据传入的钥匙的文本加载对应的钥匙
     * 
     * @param key 钥匙文本
     * @param keyType  钥匙类型
     */
    void load(const std::string& key,RsaKeyType keyType);

    /**
     * @brief 生成RSA密钥对
     * 
     */
    void generate();

    /**
     * @brief 获取密钥文本
     * 
     * @return std::string 
     */
    const std::string& getPrivateKeyStr();

    /**
     * @brief 获取公钥对象
     * 
     * @return std::string 
     */
    const std::string& getPublicKeyStr();
    /**
     * @brief 获取密钥对象
     * 
     * @return EVP_PKEY_SPTR 
     */
    EVP_PKEY* getPrivateKey();

    /**
     * @brief 获取公钥对象
     * 
     * @return EVP_PKEY_SPTR 
     */
    EVP_PKEY* getPublicKey();

    /**
     * @brief 加密
     *
     * @param plainText  明文
     * @param key  加密钥匙
     * @param isPublic  是否使用公钥加密 否则使用密钥加密
     * @return std::string
     */
    std::string encryptWithRsa(std::string_view plainText);

    /**
     * @brief 解密
     *
     * @param cipherText 密文
     * @param key 解密钥匙
     * @param isPrivate  是否使用密钥解密 否则使用公钥解密
     * @return std::string
     */
    std::string decryptWithRsa(std::string_view cipherText);

    std::string signWithPrivateKey(std::string_view plainText);

    bool signVerifyWithPublicKey(std::string_view plainText, std::string_view signTextVerify);

    std::string base64(std::string_view str);

    // void sslErrorHandle( const std::string& prefixErrorStr );

    ~RSAUtil() = default;

private:
    void generateRsaKey();
    void generatePrivateKey();
    void generatePubKey();
    void generatePrivateKeyStr();
    void generatePubKeyStr();
    EVP_PKEY_UPTR pubKey_;
    EVP_PKEY_UPTR privateKey_;
    EVP_PKEY_UPTR rsaKey_;
    std::string pubKeyStr_;
    std::string privateKeyStr_;
};