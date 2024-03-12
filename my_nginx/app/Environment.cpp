//
// Created by Administrator on 2024-03-11.
//

#include "app/Environment.h"
#include "unistd.h"
#include <cstring>
#include <iostream>
using  namespace  std;

Environment::Environment(int argc, char* argv[]) {
    int32_t envLen = 0;
    for(int i =0; environ[i]; i++){
        string* str = new string(environ[i]);
        environemnts.push_back(str);
        envLen = envLen + str->length() + 1;
    }
    memset(environ[0],0,envLen);
    for(int i = 0; i < argc; i++){
        string* arg = new string(argv[i]);
        programArgs.push_back(arg);
    }
    for(const auto& item : programArgs){
        std::cout << *item << std::endl;
    }
}

Environment *Environment::getInstance(int argc, char* argv[]) {
    static Environment environment(argc,argv);
    return &environment;
}


Environment::~Environment() {
    for(const auto& item : environemnts){
        delete item;
    }
    for(const auto& item : programArgs){
        delete item;
    }
}
