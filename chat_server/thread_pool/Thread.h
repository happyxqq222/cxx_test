//
// Created by Administrator on 2024-04-24.
//

#ifndef THREADPOOL_TEST_THREAD_H
#define THREADPOOL_TEST_THREAD_H

#include <thread>
#include <functional>
#include <string>

#include "Semaphore.h"
#include "noncopyable.h"

namespace pool{
    class Thread;
}

class Thread :  noncopyable{
public:
    using ThreadFunc = std::function<void(size_t threadIndex)>;
    explicit Thread(ThreadFunc threadFunc,size_t threadIndex);
    void setThreadName(const std::string& name);
    void start();
    Thread(Thread&&);
    ~Thread();
private:
    size_t threadIndex_;
    std::thread th_;
    std::string threadName_;
    std::unique_ptr<Semaphore> semaphore;
    ThreadFunc threadFunc_;
};


#endif //THREADPOOL_TEST_THREAD_H
