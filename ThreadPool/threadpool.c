//
// Created by Administrator on 12/30/2023.
//

#include "threadpool.h"

#define NUMBER 2

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
    pthread_cond_t fullCond;     //判断是否满了
    pthread_cond_t emptyCond;     //判断是否空了
    int shutdown;                //是否要销毁线程池 销毁为1  不销毁为0
} ThreadPool;

/**
 * 创建线程池
 * @param min
 * @param max
 * @param queueCapactiy
 * @return
 */
ThreadPool *threadPoolCreate(int min, int max, int queueCapactiy) {

    ThreadPool *pool = (ThreadPool *) malloc(sizeof(ThreadPool));
    do {
        if (pool == NULL) {
            printf("malloc threadpool fail...\n");
            break;
        }

        pool->threadIDs = (pthread_t *) malloc(sizeof(pthread_t) * max);
        if (pool->threadIDs == NULL) {
            printf("malloc threadIDs fail...\n");
            break;
        }
        memset(pool->threadIDs, 0, sizeof(pthread_t) * max);
        pool->minNUm = min;
        pool->maxNum = max;
        pool->busyNum = 0;
        pool->liveNum = 0;
        pool->exitNum = 0;

        if (pthread_mutex_init(&pool->mutexPool, NULL) != 0 ||
            pthread_mutex_init(&pool->mutexBusy, NULL) != 0 ||
            pthread_cond_init(&pool->emptyCond, NULL) != 0 ||
            pthread_cond_init(&pool->fullCond, NULL) != 0) {
            printf("mutex or cond init fail...\n");
            break;
        }

        //初始化任务队列
        pool->taskQ = (Task *) malloc(sizeof(Task) * queueCapactiy);
        if (pool->taskQ == NULL) {
            printf("queue create fail....\n");
            break;
        }
        pool->queueCapacity = queueCapactiy;
        pool->queueSize = 0;
        pool->queueFront = 0;
        pool->queueRear = 0;
        pool->shutdown = 0;

        //create manager thread
        pthread_create(&pool->managerID, NULL, manager, pool);

        //create worker thread
        for (int i = 0; i < min; i++) {
            pthread_create(&pool->threadIDs[i], NULL, worker, pool);
            pool->liveNum++;
        }
        return pool;
    } while (0);

    //free resource
    if (pool->threadIDs) free(pool->threadIDs);
    if (pool->taskQ) free(pool->taskQ);
    if (pool) free(pool);
}

/**
 * 工作线程
 * @param arg
 * @return
 */
void *worker(void *arg) {
    ThreadPool *pool = (ThreadPool *) arg;
    while (1) {
        pthread_mutex_lock(&pool->mutexPool);
        while (pool->queueSize == 0 && !pool->shutdown) {
            //block work thread
            pthread_cond_wait(&pool->emptyCond, &pool->mutexPool);
            //判断是否要销毁线程
            if (pool->exitNum > 0) {
                pool->exitNum--;
                if (pool->liveNum > pool->minNUm) {
                    pool->liveNum--;
                    pthread_mutex_unlock(&pool->mutexPool);
                    threadExit(pool);
                }
            }
        }
        //判断线程是否被关闭了
        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->mutexPool);
            threadExit(pool);
        }
        //从任务队列中取出一个任务
        Task task;
        task.func = pool->taskQ[pool->queueFront].func;
        task.arg = pool->taskQ[pool->queueFront].arg;
        //移动头节点
        pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
        pool->queueSize--;

        //wake up producer
        pthread_cond_signal(&pool->fullCond);
        pthread_mutex_unlock(&pool->mutexPool);

        printf("thread %ld start working...\n",pthread_self());
        //对busyNum++
        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mutexBusy);
        //执行任务
        task.func(task.arg);
        //release arg resource
        free(task.arg);
        task.arg = NULL;

        printf("thread %ld end working...\n",pthread_self());
        //对busyNum--
        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexBusy);
    }
    return NULL;
}

/**
 * 管理线程函数
 * @param arg
 * @return
 */
void *manager(void *arg) {
    ThreadPool *pool = (ThreadPool *) arg;
    while (!pool->shutdown) {
        sleep(3);
        //取出线程池中任务的数量和当前线程的数量
        pthread_mutex_lock(&pool->mutexPool);
        int queueSize = pool->queueSize;
        int liveNum = pool->liveNum;
        pthread_mutex_unlock(&pool->mutexPool);

        pthread_mutex_lock(&pool->mutexBusy);
        int busyNum = pool->busyNum;
        pthread_mutex_unlock(&pool->mutexBusy);

        //添加线程
        //任务个数>存活的线程个数 && 存活的线程数 < 最大线程数
        if (queueSize > liveNum && liveNum < pool->maxNum) {
            int counter = 0;
            pthread_mutex_lock(&pool->mutexPool);
            for (int i = 0; i < pool->maxNum && counter < NUMBER && pool->liveNum < pool->maxNum; ++i) {
                if (pool->threadIDs[i] == 0) {
                    pthread_create(&pool->threadIDs[i], NULL, worker, pool);
                    counter++;
                    pool->liveNum++;
                }
            }
            pthread_mutex_unlock(&pool->mutexPool);
        }

        //销毁线程
        //忙的线程数*2 < 存活的线程数 && 存活的线程 > 最小线程数
        if (busyNum * 2 < liveNum && liveNum > pool->minNUm) {
            pthread_mutex_lock(&pool->mutexPool);
            pool->exitNum = NUMBER;
            pthread_mutex_unlock(&pool->mutexPool);
            //让工作线程自杀
            for (int i = 0; i < NUMBER; ++i) {
                pthread_cond_signal(&pool->emptyCond);
            }
        }

    }
}

/**
 * thread exit function
 * @param pool
 */
void threadExit(ThreadPool *pool) {
    pthread_t tid = pthread_self();
    for (int i = 0; i < pool->maxNum; ++i) {
        pool->threadIDs[i] = 0;
        printf("threadExit() called, %ld exiting...\n", tid);
        break;
    }
    pthread_exit(NULL);
}

/**
 * add worker to thread pool
 * @param pool
 * @param func
 * @param arg
 */
void addWorker(ThreadPool *pool, void(*func)(void *), void *arg) {
    pthread_mutex_lock(&pool->mutexPool);
    while (pool->queueSize == pool->queueCapacity && !pool->shutdown) {
        //阻塞生产者线程
        pthread_cond_wait(&pool->fullCond, &pool->mutexPool);
    }
    if (pool->shutdown) {
        pthread_mutex_unlock(&pool->mutexPool);
        return;
    }
    //add worker to queue tail
    pool->taskQ[pool->queueRear].func = func;
    pool->taskQ[pool->queueRear].arg = arg;
    pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity;
    pool->queueSize++;

    //wake up
    pthread_cond_signal(&pool->emptyCond);
    pthread_mutex_unlock(&pool->mutexPool);
}

//获取线程池中工作的线程个数
int threadPoolBusyNum(ThreadPool *pool) {
    pthread_mutex_lock(&pool->mutexBusy);
    int busyNum = pool->busyNum;
    pthread_mutex_unlock(&pool->mutexBusy);
    return busyNum;
}

//获取线程池中活着的线程个数
int threadPoolAliveNum(ThreadPool *pool) {
    pthread_mutex_lock(&pool->mutexPool);
    int aliveNum = pool->liveNum;
    pthread_mutex_unlock(&pool->mutexPool);
    return aliveNum;
}

//销毁线程池
int threadPoolDestroy(ThreadPool *pool) {
    if (pool == NULL)
        return -1;
    //关闭线程池
    pool->shutdown = 1;
    //wait for recylce manager thread
    pthread_join(pool->managerID, NULL);
    //唤醒消费者线程
    pthread_cond_broadcast(&pool->emptyCond);

    //wait for recycle worker thread
    for (int i = 0; i < pool->maxNum; ++i) {
        if (pool->threadIDs[i] != 0) {
            //阻塞等待回收
            pthread_join(pool->threadIDs[i], NULL);
        }
    }

    //free pool resource
    if (pool->taskQ) {
        free(pool->taskQ);
    }
    if (pool->threadIDs) {
        free(pool->threadIDs);
    }
    pthread_mutex_destroy(&pool->mutexPool);
    pthread_mutex_destroy(&pool->mutexBusy);
    pthread_cond_destroy(&pool->emptyCond);
    pthread_cond_destroy(&pool->emptyCond);
    free(pool);

    pool = NULL;

    return 0;
}


