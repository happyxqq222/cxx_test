//
// Created by Administrator on 2024-04-24.
//

#ifndef THREADPOOL_TEST_SEMAPHORE_H
#define THREADPOOL_TEST_SEMAPHORE_H

#include <mutex>
#include <condition_variable>

class Semaphore {

public:
    Semaphore(int resLimit = 0);
    void post();
    void wait();
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    ~Semaphore() = default;

private:
    int resLimit_;
    std::mutex mutex_;
    std::condition_variable cv_;
};


#endif //THREADPOOL_TEST_SEMAPHORE_H
