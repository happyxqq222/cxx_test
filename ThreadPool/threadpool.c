//
// Created by Administrator on 12/30/2023.
//

#include <malloc.h>
#include "threadpool.h"

typedef void *(*thread_func)(void *arg);

//任务结构体
typedef struct Task {
    thread_func func;
    void *arg;
} Task;

//线程池结构体
typedef struct ThreadPool {
    //任务队列
    Task *taskQ;
    int queueCapacity;  //容量
    int queueSize;      //当前任务个数
    int queueFront;     //队头
    int queueRear;      //队尾
    pthread_t managerID;  //管理线程ID
    pthread_t *threadIDs;   //工作线程ID
    int minNUm;             //最小线程数
    int maxNum;            //最大线程数
    int busyNum;            //忙的线程个数
    int liveNum;            //存活的线程个数
    int exitNum;            //杀死的线程个数
    pthread_mutex_t mutexPool;  //锁整个线程池
    pthread_mutex_t mutexBusy;  //锁busyNum变量
    pthread_cond_t isFull;     //判断是否满了
    pthread_cond_t isEmpty;     //判断是否空了
    int shutdown;                //是否要销毁线程池 销毁为1  不销毁为0
} ThreadPool;

ThreadPool *threadPoolCreate(int min, int max, int queueSize) {
    ThreadPool *pool = (ThreadPool *) malloc(sizeof(ThreadPool));
    if(pool == NULL){

    }

}
