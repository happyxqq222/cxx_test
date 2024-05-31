#include <dirent.h>
#include <google/protobuf/arena.h>
#include <muduo/base/BlockingQueue.h>
#include <muduo/base/Logging.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/http/HttpContext.h>
#include <muduo/net/http/HttpResponse.h>
#include <muduo/net/http/HttpServer.h>
#include <oneapi/tbb/concurrent_hash_map.h>
#include <oneapi/tbb/concurrent_unordered_map.h>
#include <openssl/types.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/thread.hpp>
#include "EncryptionTransfer/net/Server.h"
#include "EncryptionTransfer/tool/AESUtils.h"
#include <tbb/concurrent_hash_map.h>
#include <EncryptionTransfer/net/UserUtil.h>
#include <EncryptionTransfer/handler/MessageDecode.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <fmt/std.h>

using namespace std;

struct S {
    int  a;
    char b;
    int  c;
    char d;
};

using namespace boost::asio;
using namespace muduo;
using namespace muduo::net;

void onRequest(const HttpRequest& request, HttpResponse* response) {
    response->setStatusCode(HttpResponse::k200Ok);
    response->setStatusMessage("OK");
    response->setContentType("text/plain");
    response->addHeader("Serveer", "Muduo");
    response->setBody("hello muduo Http Server!");
}


using Map =  tbb::concurrent_unordered_map<int,int>;
Map m;

/* template<>
struct std::default_delete<EVP_MD_CTX> {
    void operator()(EVP_MD_CTX* __ptr) const{
        cout << "release" << endl;
        EVP_MD_CTX_free(__ptr);
    }
}; */

/* void shaTest(const string_view input) {
    unique_ptr<EVP_MD_CTX> context = unique_ptr<EVP_MD_CTX>(EVP_MD_CTX_new());
    if(context == nullptr) {
        throw std::runtime_error("failed to create context");
    }
    const EVP_MD* md = EVP_sha256();
    if(EVP_DigestInit_ex(context.get(), md,nullptr) != 1) {
        throw std::runtime_error("failed to initialize digest");
    }
    if(EVP_DigestUpdate(context.get(), input.data(), input.size()) != 1) {
        throw std::runtime_error("failed to update digest");
    }
    vector<unsigned char> hash(EVP_MAX_MD_SIZE);
    unsigned int lengthOfHash = 0;
    if(EVP_DigestFinal_ex(context.get(), hash.data(), &lengthOfHash) != 1) {
        throw std::runtime_error("Failed to finalize digest");
    }
    cout << lengthOfHash << endl;
    string base64 =  base64_encode(hash.data(),lengthOfHash,false);
    cout << base64 << endl;
}
 */
int main() {
    int len = sizeof(MessageDecode);
    LOG_INFO << "主线程:" << pthread_self() ;
    Server server;
    server.startServer();
    return 0;
}


