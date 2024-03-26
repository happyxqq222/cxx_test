//
// Created by Administrator on 2024-03-25.
//
#include "Pooler.h"

#include <stdlib.h>

Pooler* Pooler::newDefaultPooler(EventLoop *loop){
    if(::getenv("MUDUO_USE_POLL")){
        return nullptr;
    }else{
        return nullptr;
    };
}