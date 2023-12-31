#include "stdio.h"
#include "threadpool.h"

void taskFunc(void *arg) {
    int num = *(int *) arg;
    printf("thread %ld is working,number=%d\n", pthread_self(),num);
    sleep(1);
}

int main() {

    ThreadPool *pool = threadPoolCreate(3, 10, 100);
    for (int i = 0; i < 100; i++) {
        int *num = (int *) malloc(sizeof(int));
        *num = i + 100;
        addWorker(pool, taskFunc, num);
    }

    sleep(20);
    int aliveNum = threadPoolAliveNum(pool);
    printf("aliveNum:%d\n",aliveNum);
    threadPoolDestroy(pool);
    return 0;
}
