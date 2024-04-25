//
// Created by Administrator on 2024-04-24.
//

#include "Thread.h"
#include <iostream>

using namespace std;

Thread::Thread(Thread::ThreadFunc threadFunc, size_t threadIndex)
        : threadFunc_(threadFunc), threadIndex_(threadIndex),semaphore(make_unique<Semaphore>()) {
}

Thread::Thread(Thread &&other)
        : th_(std::move(other.th_))
        , threadFunc_(std::move(other.threadFunc_))
        , threadIndex_(other.threadIndex_)
        , semaphore(std::move(other.semaphore)){
}


void Thread::start() {
    th_ = std::thread([this] {
        semaphore->wait();
        threadFunc_(this->threadIndex_);

    });
    th_.detach();
    semaphore->post();
}

Thread::~Thread() {
    cout << "threadindex:" << threadIndex_ << "exit" << endl;
}

void Thread::setThreadName(const std::string &name) {
    this->threadName_ =  name;
}

