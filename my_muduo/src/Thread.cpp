//
// Created by Administrator on 2024-03-28.
//

#include "Thread.h"
#include "CurrentThread.h"

#include <semaphore.h>

using namespace std;

atomic_int Thread::numCreate_{0};



void Thread::start() {
    started_ = true;
    sem_t sem;
    sem_init(&sem,false,0);
    thread_ = make_shared<thread>([&]() {
        tid_= CurrentThread::tid();
        sem_post(&sem);
        func_();
    });
    sem_wait(&sem);
}

void Thread::join() {
    joined_ = true;
    thread_->join();
}

void Thread::setDefaultName() {
    int num = ++numCreate_;
    if (name_.empty()) {
        name_.append("Thread_").append(to_string(num));
    }
}

Thread::~Thread() {
    if (started_ && !joined_) {
        thread_->detach();
    }
}
