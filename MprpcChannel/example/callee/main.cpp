#include <iostream>
#include <memory>
#include <string_view>
#include <vector>
#include "MprpcApplication.h"
#include "RpcProvider.h"
#include "callee/UserService.h"
#include "user.pb.h"

using namespace std;

int main(int argc, char** argv) {
    vector<string_view> args;
    MprpcApplication::init("config.json");
    MprpcApplication& application = MprpcApplication::getInstance();
    RpcProvider provider;
    UserService userService;
    service::LoginRequest loginRequest;
    provider.registerService(make_shared<UserService>());
    provider.run();
    cout << "callee" << endl;
    return 0;
}