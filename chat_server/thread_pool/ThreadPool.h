//
// Created by Administrator on 2024-04-24.
//

#ifndef THREADPOOL_TEST_THREADPOOL_H
#define THREADPOOL_TEST_THREADPOOL_H

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <memory>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <variant>

#include "Thread.h"
#include "Task.h"
#include "Any.h"
#include "Result.h"
#include "TimeUnit.h"
#include "Volatile.h"


namespace pool {
    class ThreadPool;
    enum class PoolMode;
}

enum class PoolMode{
    Fixed,
    Cached,
};

class ThreadPool {

public:
    template<Unit unit>
    ThreadPool(size_t threadSize,size_t maximumSize,TimeUnit<unit> timeUnit);
    std::shared_ptr<Result> submit(std::shared_ptr<Task> task);
    void setPoolMode(PoolMode);
    void threadExec(size_t threadIndex);
    ~ThreadPool();

private:
    PoolMode poolMode_;
    ///size_t coreSize_;      //核心数量
    Volatile<size_t> coreSize_;
    Volatile<size_t> maximumSize_;  //最大数量
    size_t taskCapacity_;
    std::chrono::milliseconds keepAliveTime_;
    std::atomic_uint taskSize_;
    std::mutex mutex_;
    std::condition_variable notFull_;
    std::condition_variable notEmpty_;
    std::condition_variable exitCondition_;
    std::unordered_map<size_t,std::unique_ptr<Thread>> threads_;
    std::queue<std::shared_ptr<Task>> taskQueue_;
    std::atomic_uint threadCount_;
    std::atomic_bool isRunning;
};

template<Unit unit>
ThreadPool::ThreadPool(size_t coreSize, size_t maximumSize, TimeUnit<unit> keepAliveTime)
        : coreSize_(coreSize)
        , maximumSize_(maximumSize)
        , keepAliveTime_(*keepAliveTime.getTimeMillDuration())
        , taskSize_(0), poolMode_(PoolMode::Fixed)
        , taskCapacity_(1024)
        , threadCount_(0){
    isRunning.store(true,std::memory_order_release);
    for (int i = 0; i < coreSize; i++) {
        threadCount_.fetch_add(1);
        std::unique_ptr<Thread> threadPtr = std::make_unique<Thread>(std::bind(&ThreadPool::threadExec, this,std::placeholders::_1), threadCount_);
        Thread* currentThread = threadPtr.get();
        threads_.insert({threadCount_, std::move(threadPtr)});
        currentThread->start();
        currentThread = nullptr;
    }
}


#endif //THREADPOOL_TEST_THREADPOOL_H
