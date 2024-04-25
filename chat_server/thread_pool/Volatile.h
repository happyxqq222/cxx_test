//
// Created by Administrator on 2024-04-25.
//

#ifndef THREADPOOL_TEST_VOLATILE_H
#define THREADPOOL_TEST_VOLATILE_H

#include <atomic>
#include <barrier>

template<typename Tp>
class Volatile {
public:
    Volatile(Tp value){
        tp_ = value;
        std::atomic_thread_fence(std::memory_order_release);
    }
    void store(Tp tp){
        this->tp_ = tp;
        std::atomic_thread_fence(std::memory_order_release);
    }
    Tp& load(){
        std::atomic_thread_fence(std::memory_order_acquire);
        return tp_;
    }

private:
    Tp tp_;
};


#endif //THREADPOOL_TEST_VOLATILE_H
