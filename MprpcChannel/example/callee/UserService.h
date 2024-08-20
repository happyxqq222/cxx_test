#pragma once
#include <iostream>
#include <string>
#include "user.pb.h"
/*
**
 * UserService原来是一个本地服务，提供两个进程内的本地方法，login和
 */
class UserService : public service::UserServiceRpc {
public:
    bool checkLogin(const std::string& name, const std::string& pwd) {
        std::cout << "doing local server: Login" << std::endl;
        std::cout << "name:"
                  << "pwd :" << pwd << std::endl;
        return true;
    }

    // caller ====> Login(LoginRequest) => muduo => calle
    // callee ====> Login(LoginRequest) => 下面重写的方法上
    void Login(::google::protobuf::RpcController* controller,
               const ::service::LoginRequest* request,
               ::service::LoginResponse* response,
               ::google::protobuf::Closure* done) override {
        using namespace std;
        const string& name = request->name();
        const string& pwd = request->pwd();
        //做本地业务
        bool loginResult = this->checkLogin(name, pwd);
        //把响应返回给调用方
        response->set_success(loginResult);
        service::ResultCode* code = response->mutable_resultcode();
        code->set_errorcode(0);
        code->set_errmsg("");
        //执行回调操作，执行响应对应数据序列化和网络发送（都是由框架来完成)
        done->Run();
    }
};