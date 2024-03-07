//
// Created by Administrator on 2024-03-06.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <thread>
#include <mutex>
#include <memory>
#include <vector>
#include <list>
#include <condition_variable>

class Task{
public:
    virtual int run() = 0;

};


class ThreadPool {

public:
    /**
     * init thread num
     * @param num
     */
    void init(int num);

    /**
     * start all thread ,first call init
     */
    void start();

    void Stop();

    void addTask(Task* Task);

    Task* getTask();

    bool isExist(){return is_exit;}



private:
    void run();
    int thread_num = 0;
    std::mutex m;
    std::vector<std::thread*> threads;
    std::list<Task*> tasks;
    std::condition_variable cond;
    bool is_exit = false;
};


#endif //THREADPOOL_THREADPOOL_H
