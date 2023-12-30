//
// Created by Administrator on 12/30/2023.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H
#include <pthread.h>

typedef struct ThreadPool ThreadPool;

//创建线程池并初始化

ThreadPool* threadPoolCreate(int min,int max,int queueSize);

//销毁线程池


//给线程池添加任务

//获取线程池中工作的线程个数

//获取线程池中活着的线程个数

#endif //THREADPOOL_THREADPOOL_H
