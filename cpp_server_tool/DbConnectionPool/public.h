//
// Created by Administrator on 2024-02-25.
//

#ifndef CPP_SERVER_TOOL_PUBLIC_H
#define CPP_SERVER_TOOL_PUBLIC_H
#include <iostream>

#define LOG(str) \
   std::cout << __FILE__ << ":" << __LINE__ << " " << \
    __TIMESTAMP__ << " : " << str << std::endl;

#endif //CPP_SERVER_TOOL_PUBLIC_H
