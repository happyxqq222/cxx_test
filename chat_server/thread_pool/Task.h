//
// Created by Administrator on 2024-04-24.
//

#ifndef THREADPOOL_TEST_TASK_H
#define THREADPOOL_TEST_TASK_H

#include "Any.h"

namespace pool{
    class Task;
}

class Result;

class Task{
public:
    void exec();
    virtual Any run() = 0;
    void setResult(std::shared_ptr<Result>& result){
        this->result_ = result;
    }
private:
    std::weak_ptr<Result> result_;
};

#endif //THREADPOOL_TEST_TASK_H
