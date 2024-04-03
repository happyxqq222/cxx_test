#include <iostream>
#include "ChatServer.h"

using namespace std;

int main()
{
    EventLoop loop;
    InetAddress address("0.0.0.0",8888);
    ChatServer server(&loop,address,"ChatServer");
    server.startServer();
    loop.loop();
    return 0;
}