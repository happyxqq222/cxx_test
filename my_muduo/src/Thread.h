//
// Created by Administrator on 2024-03-28.
//

#ifndef MY_MUDUO_THREAD_H
#define MY_MUDUO_THREAD_H

#include "noncopyable.h"

#include <functional>
#include <thread>
#include <memory>
#include <unistd.h>
#include <string>
#include <atomic>
#include <string_view>

class Thread : noncopyable{
public:
    using ThreadFunc = std::function<void()>;

    template<typename T,
            typename=std::enable_if_t<std::is_convertible_v<T,std::string>>
    >
    explicit Thread(ThreadFunc, T&& name = "");

    ~Thread();

    void start();
    void join();

    bool started() const{
        return started_;
    }
    pid_t tid() const{
        return tid_;
    }
    const std::string& name() const{
        return name_;
    }
    static int numCreated(){
        return numCreate_;
    }
private:

    void setDefaultName();

    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> thread_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_ = "";
    static std::atomic_int numCreate_;
};

template<typename T, typename>
Thread::Thread(Thread::ThreadFunc func, T &&name)
        :started_(false),
         joined_(false),
         tid_(0),
         func_(std::move(func)),
         name_(std::forward<T &&>(name)) {
    setDefaultName();
}

#endif //MY_MUDUO_THREAD_H
