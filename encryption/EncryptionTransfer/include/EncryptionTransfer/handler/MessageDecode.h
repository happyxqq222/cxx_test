#pragma  once
#include <muduo/net/Callbacks.h>

class MessageDecode {

public:
   MessageDecode() = default;
   void onMessage(const muduo::net::TcpConnectionPtr& connPtr,
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
};