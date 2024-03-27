//
// Created by Administrator on 2024-03-27.
//

#include "CurrentThread.h"
#include <unistd.h>
#include <sys/syscall.h>

namespace CurrentThread
{
    __thread int t_cachedTid = 0;

    thread_local int t_cachedTid2 = 0;

    void cacheTid() {
        if(t_cachedTid == 0){
            //通过Linux系统调用获取当前的值
            t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
        }
    }

    void cacheTid2() {
        if(t_cachedTid2 == 0){
            t_cachedTid2 = static_cast<pid_t>(::syscall(SYS_gettid));
        }
    }


}