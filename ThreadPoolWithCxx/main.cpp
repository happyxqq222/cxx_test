//
// Created by Administrator on 12/31/2023.
//
#include <iostream>
#include <atomic>
#include "ThreadPool.h"

using namespace std;


class MyTask : public Task
{
public:
    int run() override{
        cout << "Mytask: " << name << endl;
        return 0;
    }
    string name = "";
};

int main() {
    ThreadPool pool;
    pool.init(16);
    pool.start();
    MyTask task;
    task.name = "test name 001";
    pool.addTask(&task);
    getchar();
    return 1;
}