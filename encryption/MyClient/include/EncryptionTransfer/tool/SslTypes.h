#pragma once

#include <openssl/evp.h>
#include <openssl/types.h>
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <stdexcept>
#include <string_view>
#include <string>
#include <cstdio>
#include <memory>

using EVP_PKEY_SPTR = std::shared_ptr<EVP_PKEY>;
using EVP_PKEY_UPTR = std::unique_ptr<EVP_PKEY>;

template<>
struct std::default_delete<EVP_PKEY > {
    void operator()(EVP_PKEY* __ptr) const{
        EVP_PKEY_free(__ptr);
    }
};

template<>
struct std::default_delete<EVP_PKEY_CTX  > {
    void operator()(EVP_PKEY_CTX * __ptr) const{
        EVP_PKEY_CTX_free(__ptr);
    }
};

template<>
struct std::default_delete<FILE > {
    void operator()(FILE* __ptr) const{
        fclose(__ptr);
    }
};

template<>
struct std::default_delete<BIO> {
    void operator()(BIO* bio) const {
        BIO_free(bio);
    }
};

template<>
struct std::default_delete<EVP_CIPHER_CTX > {
    void operator()(EVP_CIPHER_CTX* ctx) const {
        EVP_CIPHER_CTX_free(ctx);
    }
};

template<>
struct std::default_delete<EVP_MD_CTX > {
    void operator()(EVP_MD_CTX* ctx) const {
        EVP_MD_CTX_free(ctx);
    }
};


inline void sslErrorHandle( std::string_view prefixErrorStr ) {
    std::string errorStr( 512, 0 );
    ERR_error_string_n( ERR_get_error(), errorStr.data(), errorStr.size() );
    std::string errorDesc;
    errorDesc.append(prefixErrorStr).append(errorStr);
    throw std::runtime_error(errorDesc);
}