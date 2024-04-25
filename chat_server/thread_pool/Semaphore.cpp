//
// Created by Administrator on 2024-04-24.
//

#include "Semaphore.h"
#include <atomic>

Semaphore::Semaphore(int resLimit) :resLimit_(resLimit){
}

void Semaphore::post() {
    std::unique_lock<std::mutex> lock(mutex_);
    resLimit_++;
    std::atomic_thread_fence(std::memory_order_release);
    cv_.notify_one();
}

void Semaphore::wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock,[this]{
        std::atomic_thread_fence(std::memory_order_acquire);
        return resLimit_ > 0 ;
    });
    resLimit_--;
    std::atomic_thread_fence(std::memory_order_release);
}
