//
// Created by Administrator on 2024-03-06.
//

#include "ThreadPool.h"
#include <iostream>

using namespace std;

void ThreadPool::init(int num) {
    unique_lock<mutex> lock(m);
    this->thread_num = num;
    cout << "Thread pool init :" << num << endl;
}

void ThreadPool::start() {
    unique_lock<mutex> lock(m);
    if(thread_num <= 0){
        cerr << "Please init threadpool" << endl;
        return;
    }
    if(!threads.empty()){
        cerr << "Thread pool has started" << endl;
        return;
    }
    for(int i = 0; i < thread_num; i++){
        auto th = new thread(&ThreadPool::run, this);
        threads.push_back(th);
    }
}

void ThreadPool::Stop() {
    unique_lock<mutex> lock(m);
    is_exit = true;
    cond.notify_all();
    for(auto & th : threads){
        th->join();
        delete th;
    }
    threads.clear();

}


void ThreadPool::run() {
    cout << "begin threadpool run " << this_thread::get_id() << endl;
    while(!is_exit){
        auto task = getTask();
        if(!task) continue;
        try{
            task->run();
        }catch(...){

        }
    }
    cout << "end threadpool run " << this_thread::get_id() << endl;
}


void ThreadPool::addTask(Task *task) {
    unique_lock<mutex> lock(m);
    tasks.push_back(task);
}

Task *ThreadPool::getTask() {
    unique_lock<mutex> lock(m);
    while(tasks.empty() && !is_exit){
        cond.wait(lock);
    }
    if(tasks.empty())
        return nullptr;
    Task* task =  tasks.front();
    tasks.pop_front();
    return task;
}
