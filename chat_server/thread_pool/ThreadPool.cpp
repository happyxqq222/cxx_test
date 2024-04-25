//
// Created by Administrator on 2024-04-24.
//

#include <chrono>

#include "ThreadPool.h"

using namespace std;



shared_ptr<Result> ThreadPool::submit(std::shared_ptr<Task> task) {
    std::unique_lock<mutex> lock(mutex_);
    //任务比较多，创建一些新的线程
    if(taskQueue_.size() > 0 && threads_.size() < maximumSize_.load()){
        threadCount_++;
        std::unique_ptr<Thread> threadPtr = make_unique<Thread>(std::bind(&ThreadPool::threadExec, this,std::placeholders::_1), threadCount_);
        Thread* currentThread = threadPtr.get();
        threads_.insert({threadCount_, std::move(threadPtr)});
        currentThread->start();
    }

    bool retFull = notFull_.wait_for(lock,1s,[this]()->bool{
        return taskQueue_.size() < taskCapacity_;
    });

    if(!retFull){
        throw "queue is full";
    }
    shared_ptr<Result> result  = make_shared<Result>(task);
    task->setResult(result);

    taskQueue_.emplace(task);
    taskSize_.fetch_add(1,std::memory_order_release);
    notEmpty_.notify_one();
    return result;
}

void ThreadPool::setPoolMode(PoolMode poolMode) {
    this->poolMode_ = poolMode;
}

void ThreadPool::threadExec(size_t threadIndex) {
    while (isRunning.load(std::memory_order_acquire)) {
        shared_ptr<Task> task;
        {
            std::unique_lock<mutex> lock(mutex_);
            cout << threadIndex << ",尝试获取任务" << endl;
            notEmpty_.wait_for(lock,keepAliveTime_,[this]()->bool{
                return taskQueue_.size() > 0 || !isRunning.load(memory_order_acquire);
            });
            cout << threadIndex << ",尝试获取任务后:" << isRunning << endl;
            if(!isRunning){  //线程池停止状态
                threads_.erase(threadIndex);
                cout << "线程数量:" << threads_.size() << endl;
                break;
            }else if(taskQueue_.size()  == 0){
                if(threads_.size() > coreSize_.load()){
                    threads_.erase(threadIndex);
                    break; //跳出循环结束当前线程
                }
            }else{
                task = taskQueue_.front();
                taskQueue_.pop();
                taskSize_.fetch_sub(1,std::memory_order_release);
                notFull_.notify_one();
            }
        }
        if(task){
            task->exec();
            if(!isRunning){
                unique_lock<mutex> lock2(mutex_);
                threads_.erase(threadIndex);
            }
        }
    }
    if(!isRunning && threads_.size() == 0){
        cout << "线程全部清除:" << threads_.size() << endl;
        exitCondition_.notify_all();
    }

}

ThreadPool::~ThreadPool() {
    unique_lock<mutex> lock(mutex_);
    isRunning.store(false,std::memory_order_release);
    notEmpty_.notify_all();
    exitCondition_.wait(lock,[this]{
       return threads_.size() == 0;
    });
    cout << "~ThreadPool" << endl;
}

