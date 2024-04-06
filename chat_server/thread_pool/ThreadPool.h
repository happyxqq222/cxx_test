//
// Created by Administrator on 2024-04-05.
//

#ifndef CHAT_SERVER_THREADPOOL_H
#define CHAT_SERVER_THREADPOOL_H

#include <functional>
#include <vector>
#include <stdexcept>
#include <thread>

class ThreadPool {
private:
    unsigned short _initSize;  //初始化线程数量
    using Task = std::function<void()>; //定义类型
};


#endif //CHAT_SERVER_THREADPOOL_H
