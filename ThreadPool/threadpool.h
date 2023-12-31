//
// Created by Administrator on 12/30/2023.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H
#include <pthread.h>
#include <memory.h>
#include <malloc.h>
#include <unistd.h>

typedef void (*thread_func)(void *arg);

typedef struct ThreadPool ThreadPool;

//创建线程池并初始化
ThreadPool* threadPoolCreate(int min,int max,int queueCapacity);


//销毁线程池
int threadPoolDestroy(ThreadPool* pool);


//给线程池添加任务
void addWorker(ThreadPool* pool, void(*func)(void*),void* arg);

//获取线程池中工作的线程个数
int threadPoolBusyNum(ThreadPool* pool);

//获取线程池中活着的线程个数
int threadPoolAliveNum(ThreadPool* pool);

//工作线程
void* worker(void* arg);

//管理线程
void* manager(void* arg);

//thread pool exit
void threadExit(ThreadPool* pool);

#endif //THREADPOOL_THREADPOOL_H
