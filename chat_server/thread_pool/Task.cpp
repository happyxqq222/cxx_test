//
// Created by Administrator on 2024-04-25.
//
#include "Task.h"
#include "Result.h"

#include <thread>

void Task::exec() {
    Any any = this->run();
    if(any.isEmpty()){
        std::cout << "any is empty" << std::endl;
    }
    auto r = result_.lock();  //判断是否能提升成功，如果提升失败说明在别的线程中销毁了
    if(r != nullptr){
        r->setVal(any);
    }

}
