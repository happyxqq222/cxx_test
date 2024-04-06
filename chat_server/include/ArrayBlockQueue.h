//
// Created by Administrator on 2024-04-06.
//

#ifndef CHAT_SERVER_ARRAYBLOCKQUEUE_H
#define CHAT_SERVER_ARRAYBLOCKQUEUE_H
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ArrayBlockQueue{
private:
    std::queue<T> queue_;
    std::mutex mtx_;
    std::condition_variable cv_producer_;
    std::condition_variable cv_consumer_;
    size_t capacity_;
    bool closed_ = false;

public:
    ArrayBlockQueue(size_t capacity = 16) : capacity_(capacity){}

    bool send(T value){
        std::unique_lock<std::mutex> lock(mtx_);
        cv_producer_.wait(lock,[this](){
            return (queue_.size() < capacity_) || closed_;
        });
        if(closed_){
            return false;
        }
        queue_.push(value);
        cv_consumer_.notify_one();
        return true;
    }

    bool receive(T& value){
        std::unique_lock<std::mutex> lock(mtx_);
        cv_consumer_.wait(lock,[this](){
           return !queue_.empty() || closed_;
        });
        if(closed_ && queue_.empty()){
            return false;
        }
        value = queue_.front();
        queue_.pop();
        cv_producer_.notify_one();
        return true;
    }

    void close(){
        std::unique_lock<std::mutex> lock(mtx_);
        closed_ = true;
        cv_producer_.notify_all();
        cv_consumer_.notify_all();
    }
};

#endif //CHAT_SERVER_ARRAYBLOCKQUEUE_H
