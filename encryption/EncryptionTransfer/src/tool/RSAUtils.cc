#include "EncryptionTransfer/tool/RSAUtils.h"
#include "EncryptionTransfer/tool/SslTypes.h"

#include <cstddef>
#include <memory>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/types.h>
#include <stdexcept>
#include <string>
#include <string_view>


using namespace std;

RSAUtil::RSAUtil() {

}


void RSAUtil::generate() {
    generateRsaKey();
    generatePrivateKey();
    generatePubKey();
    generatePrivateKeyStr();
    generatePubKeyStr();
}

void RSAUtil::load(const std::string& key,RsaKeyType keyType) {
    unique_ptr<BIO> bio = unique_ptr<BIO>(BIO_new_mem_buf(key.data(), key.size()));
    if(!bio) {
        sslErrorHandle("load BIO_new_mem_buf error");
    }
    if(keyType == RsaKeyType::PRIVATE) {
        privateKey_ = std::unique_ptr<EVP_PKEY>(PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr, nullptr));
        if(!privateKey_) {
            sslErrorHandle("PEM_read_bio_PrivateKey error");
        }
        privateKeyStr_ = key;
    } else {
        pubKey_ = std::unique_ptr<EVP_PKEY>(PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr));
        if(!pubKey_) {
            sslErrorHandle("PEM_read_bio_PUBKEY error");
        }
        pubKeyStr_ = key;
    }
}


void RSAUtil::generateRsaKey() {

    //创建EVP密钥对象
    std::unique_ptr< EVP_PKEY_CTX > ctx = std::unique_ptr< EVP_PKEY_CTX >( EVP_PKEY_CTX_new_id( EVP_PKEY_RSA, nullptr ) );
    if ( !ctx ) {
        sslErrorHandle( "EVP_PKEY_CTX_new_id" );
    }
    //初始化密钥生成参数
    if ( EVP_PKEY_keygen_init( ctx.get() ) <= 0 ) {
        sslErrorHandle( "EVP_PKEY_keygen_init" );
    }
    //设置密钥位数
    if ( EVP_PKEY_CTX_set_rsa_keygen_bits( ctx.get(), 2048 ) <= 0 ) {
        sslErrorHandle( "EVP_PKEY_CTX_set_rsa_keygen_bits" );
    }
    //生成密钥对
    EVP_PKEY* key = nullptr;
    if ( EVP_PKEY_keygen( ctx.get(), &key ) <= 0 ) {
        sslErrorHandle( "EVP_PKEY_keygen" );
    }
    rsaKey_ = std::unique_ptr< EVP_PKEY >( key);
}

void RSAUtil::generatePrivateKey() {
    unique_ptr< BIO > privateBio = unique_ptr< BIO >( BIO_new( BIO_s_mem() ) );
    if ( !privateBio ) {
        throw std::runtime_error( "privateBIO new" );
    }
    if ( !PEM_write_bio_PrivateKey( privateBio.get(), rsaKey_.get(), nullptr, nullptr, 0, nullptr, 0 ) ) {
        throw std::runtime_error( "PEM_write_bio_PrivateKey error" );
    }
    EVP_PKEY* key = PEM_read_bio_PrivateKey( privateBio.get(), nullptr, nullptr, nullptr );
    privateKey_ =  unique_ptr< EVP_PKEY >( key);
}


void RSAUtil::generatePubKey() {
    unique_ptr< BIO > publicBio = unique_ptr< BIO >( BIO_new( BIO_s_mem() ) );
    if ( !publicBio ) {
        throw std::runtime_error( "privateBIO new error" );
    }
    if ( !PEM_write_bio_PUBKEY( publicBio.get(), rsaKey_.get() ) ) {
        throw std::runtime_error( "PEM_write_bio_PUBKEY error" );
    }
    EVP_PKEY* key = PEM_read_bio_PUBKEY( publicBio.get(), nullptr, nullptr, nullptr );
    pubKey_ =  unique_ptr<EVP_PKEY>( key);
}

void RSAUtil::generatePrivateKeyStr() {
    unique_ptr< BIO > privateBio = unique_ptr< BIO >( BIO_new( BIO_s_mem() ) );
    if ( !privateBio ) {
        throw std::runtime_error( "privateBIO new" );
    }
    if ( !PEM_write_bio_PrivateKey( privateBio.get(), privateKey_.get(), nullptr, nullptr, 0, nullptr, 0 ) ) {
        throw std::runtime_error( "PEM_write_bio_PrivateKey error" );
    }
    char* privateStr;
    BIO_get_mem_data( privateBio.get(), &privateStr );
    privateKeyStr_ = privateStr;
}

void RSAUtil::generatePubKeyStr() {
    unique_ptr< BIO > publicBio = unique_ptr< BIO >( BIO_new( BIO_s_mem() ) );
    if ( !publicBio ) {
        throw std::runtime_error( "privateBIO new error" );
    }
    if ( !PEM_write_bio_PUBKEY( publicBio.get(), pubKey_.get() ) ) {
        throw std::runtime_error( "PEM_write_bio_PUBKEY error" );
    }
    char* publicStr;
    BIO_get_mem_data( publicBio.get(), &publicStr );
    pubKeyStr_ = publicStr;
}

const string& RSAUtil::getPrivateKeyStr() {
    return privateKeyStr_;
}

const string& RSAUtil::getPublicKeyStr() {
    return pubKeyStr_;
}

EVP_PKEY* RSAUtil::getPrivateKey() {
    return privateKey_.get();
}

EVP_PKEY* RSAUtil::getPublicKey() {
    return pubKey_.get();
}

std::string RSAUtil::encryptWithRsa( string_view plainText ) {
    //创建上下文
    unique_ptr< EVP_PKEY_CTX > ctx = unique_ptr< EVP_PKEY_CTX >( EVP_PKEY_CTX_new( pubKey_.get(), nullptr ) );
    if ( !ctx ) {
        sslErrorHandle("EVP_CIPHER_CTX_new error");
    }
    //初始化加密操作
    if ( EVP_PKEY_encrypt_init( ctx.get() ) <= 0 ) {
        sslErrorHandle("EVP_PKEY_encrypt_init error");
    }

    //设置填充模式
    if ( EVP_PKEY_CTX_set_rsa_padding( ctx.get(), RSA_PKCS1_PADDING ) <= 0 ) {
        sslErrorHandle("EVP_PKEY_CTX_set_rsa_padding error");
    }

    //获取加密输出长度
    std::size_t encryptedLen = 0;
    if ( EVP_PKEY_encrypt( ctx.get(), NULL, &encryptedLen, reinterpret_cast< const unsigned char* >( plainText.data() ), plainText.size() ) <= 0 ) {
        sslErrorHandle("EVP_PKEY_encrypt getlen error");
    }

    //分配内存
    string cipherText( encryptedLen, 0 );

    //加密
    if ( EVP_PKEY_encrypt( ctx.get(), ( unsigned char* )cipherText.data(), &encryptedLen, reinterpret_cast< const unsigned char* >( plainText.data() ), plainText.size() ) <= 0 ) {
        sslErrorHandle("EVP_PKEY_encrypt error ");
    }
    return cipherText;
}

std::string RSAUtil::decryptWithRsa( string_view cipherText) {
    string errorStr( 256, 0 );
    //创建解密上下文
    unique_ptr< EVP_PKEY_CTX > ctx = unique_ptr< EVP_PKEY_CTX >( EVP_PKEY_CTX_new( privateKey_.get(), nullptr ) );
    if ( !ctx ) {
        sslErrorHandle("EVP_PKEY_CTX_new error");
    }

    //初始化解密操作
    if ( EVP_PKEY_decrypt_init( ctx.get() ) <= 0 ) {
        sslErrorHandle("EVP_PKEY_decrypt_init error");
    }

    //设置填充模式
    if ( EVP_PKEY_CTX_set_rsa_padding( ctx.get(), RSA_PKCS1_PADDING ) <= 0 ) {
        sslErrorHandle("EVP_PKEY_CTX_set_rsa_padding error");
    }


    size_t plainTextLen = 0;
    //获取输出长度
    if ( EVP_PKEY_decrypt( ctx.get(), nullptr, &plainTextLen, ( unsigned char* )cipherText.data(), cipherText.size() ) <= 0 ) {
        sslErrorHandle("EVP_PKEY_decrypt getlen error");
    }

    //分配内存
    string plainText( plainTextLen, 0 );

    //解密
    if ( EVP_PKEY_decrypt( ctx.get(), ( unsigned char* )plainText.data(), &plainTextLen, ( unsigned char* )cipherText.data(), cipherText.size() ) <= 0 ) {
        sslErrorHandle("EVP_PKEY_decrypt error");
    }
    return plainText;
}

std::string RSAUtil::signWithPrivateKey(std::string_view plainText) {
    unique_ptr<EVP_MD_CTX> ctx = unique_ptr<EVP_MD_CTX>(EVP_MD_CTX_new());
    if(!ctx) {
        sslErrorHandle("EVP_MD_CTX_new error");
    }
    //初始化签名上下文
    if(EVP_DigestSignInit(ctx.get(), nullptr, EVP_sha256(), nullptr, privateKey_.get()) <= 0) {
        sslErrorHandle("EVP_DigestSignInit error");
    }
    //更新签名数据
    if(EVP_DigestSignUpdate(ctx.get(), plainText.data(), plainText.size()) <= 0) {
        sslErrorHandle("EVP_DigestSignUpdate error");
    }
    //获取签名结果
    size_t signatureLen;
    if(EVP_DigestSignFinal(ctx.get(), nullptr, &signatureLen) <= 0) {
        sslErrorHandle("EVP_DigestSignFinal getlen error");
    }
    string signText (signatureLen,0);
    if(EVP_DigestSignFinal(ctx.get(), (unsigned char*)signText.data(), &signatureLen) < 1) {
        sslErrorHandle("EVP_DigestSignFinal error");
    }
    return signText;
}

bool RSAUtil::signVerifyWithPublicKey(std::string_view plainText,std::string_view signText) {
    unique_ptr<EVP_MD_CTX> ctx = unique_ptr<EVP_MD_CTX>(EVP_MD_CTX_new());
    if(!ctx) {
        sslErrorHandle("EVP_MD_CTX_new error");
    }
    if(EVP_DigestVerifyInit(ctx.get(), nullptr, EVP_sha256(), nullptr, pubKey_.get()) < 1) {
        sslErrorHandle("EVP_DigestVerifyInit error");
    }
    //更新验签数据
    if(EVP_DigestVerifyUpdate(ctx.get(), plainText.data(), plainText.size()) < 1) {
        sslErrorHandle("EVP_DigestVerifyUpdate error");
    }
    int verifyResult = EVP_DigestVerifyFinal(ctx.get(), (unsigned char*)signText.data(), signText.size());
    if(verifyResult < 0 ) {
        sslErrorHandle("EVP_DigestVerifyFinal error");
    }
    return verifyResult == 1;
}

std::string RSAUtil::base64(std::string_view str) {
    unique_ptr<BIO> bio = unique_ptr<BIO>(BIO_new(BIO_s_mem()));
    if(!bio) {
        sslErrorHandle("BIO_new error");
    }
    unique_ptr<BIO> b64Bio = unique_ptr<BIO>(BIO_new(BIO_f_base64()));
    if(!b64Bio){
        sslErrorHandle("BIO_new_b64 error");
    }
    BIO_set_flags(b64Bio.get(), BIO_FLAGS_BASE64_NO_NL);
    BIO_push(b64Bio.get(), bio.get());
    BIO_write(b64Bio.get(), str.data(), str.size());
    BIO_flush(b64Bio.get());
    BUF_MEM* bptr;
    BIO_get_mem_ptr(bio.get(), &bptr);
    return string(bptr->data,bptr->length);
}

// void RSAUtil::sslErrorHandle( const string& prefixErrorStr ) {
//     std::string errorStr( 512, 0 );
//     ERR_error_string_n( ERR_get_error(), errorStr.data(), errorStr.size() );
//     throw std::runtime_error( prefixErrorStr + " : " + errorStr );
// }