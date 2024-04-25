//
// Created by Administrator on 2024-04-24.
//

#include "Result.h"

Result::Result(std::shared_ptr<Task> task, bool isValid) : task_(task),isValid_(isValid),sem_(std::make_unique<Semaphore>()),any_() {
}

void Result::setVal(Any& any)  {
    this->any_ = std::move(any);
    sem_->post();
}

std::shared_ptr<Any> Result::get() {
    if(!isValid_){
        return nullptr;
    }
    sem_->wait();
    return std::make_shared<Any>(std::move(any_));
}

void Result::setVal(Any &&any) {
    this->any_ = std::move(any);
    sem_->post();
}

Result::Result(Result &&result) noexcept
        : any_(std::move(result.any_))
        , sem_(std::move(result.sem_))
        , task_(result.task_)
        , isValid_(result.isValid_.load(std::memory_order_acquire)) {

}

Result &Result::operator=(Result &&result) noexcept {
    if(this == &result){
        return *this;
    }
    this->any_  = std::move(result.any_);
    this->sem_ = std::move(result.sem_);
    this->task_ = result.task_;
    this->isValid_ = result.isValid_.load(std::memory_order_acquire);
    return *this;
}

Result::~Result() {
}
