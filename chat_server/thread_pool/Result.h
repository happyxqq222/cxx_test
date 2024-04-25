//
// Created by Administrator on 2024-04-24.
//

#ifndef THREADPOOL_TEST_RESULT_H
#define THREADPOOL_TEST_RESULT_H

#include "Any.h"
#include "Semaphore.h"
#include "Task.h"
#include <atomic>

class Result {

public:
    explicit Result(std::shared_ptr<Task> task,bool isValid = true);
    ~Result();
    void setVal(Any& any);
    void setVal(Any&& any);
    std::shared_ptr<Any> get();

    Result(const Result&) = delete;
    Result& operator=(const Result&) = delete;
    Result(Result&& result) noexcept;
    Result& operator=(Result&& result) noexcept;
private:
    Any any_;
    std::unique_ptr<Semaphore> sem_;
    std::shared_ptr<Task> task_;
    std::atomic_bool isValid_;
};


#endif //THREADPOOL_TEST_RESULT_H
