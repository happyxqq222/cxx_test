#pragma  once
#include <muduo/net/Callbacks.h>
#include "EncryptionTransfer/tool/RSAUtils.h"

class MessageDecode {

public:
   MessageDecode(RSAUtil& rsaUtil) :readStatus(ReadStatus::Header),rsaUtil_(rsaUtil){};
   void parseMessage(const muduo::net::TcpConnectionPtr& connPtr,
                  muduo::net::Buffer* buffer,
                  muduo::Timestamp timestamp);
   void onClose(const muduo::net::TcpConnectionPtr& connPtr);
   ~MessageDecode() = default;
private:
    enum class ReadStatus{
        Header,
        Conentet
    };
    ReadStatus readStatus;
    int contentLength;
    RSAUtil& rsaUtil_;
};