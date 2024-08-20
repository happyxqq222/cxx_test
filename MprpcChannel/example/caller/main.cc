#include <google/protobuf/service.h>
#include <iostream>
#include <string>
#include "MprpcApplication.h"
#include "MprpcChannel.h"
#include "user.pb.h"

using namespace std;

int main() {
    //整个程序启动以后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架的初始化函数
    MprpcApplication::init("config.json");
    // MprpcChannel rpcChannel;
    // service::UserServiceRpc_Stub  stub(&rpcChannel);
    // stub.Login();
}