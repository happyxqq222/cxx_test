//
// Created by Administrator on 2024-03-07.
//
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include "signal.h"
#include "signal/ngx_signal.h"
#include "spdlog/spdlog.h"

struct ngx_signal_t
{
    int signo;   //信号对应的数字编号，每个信号都有对应的#define,
    const char *signame;   //信号对应的中文名字,SIGHUP
    void (*handler)(int,siginfo_t*,void*);
};

static void ngx_signal_handler(int signo, siginfo_t* siginfo, void* ucontext);  //static表示该函数只在当前文件内可见

ngx_signal_t signals[]{
        {SIGHUP,"SIGHUP", ngx_signal_handler},   //终端断开信号,对于守护进程常用于reload重载配置文件通知--标识1
        {SIGINT,"SIGINT",ngx_signal_handler},    //标识2
        {SIGTERM,"SIGTERM",ngx_signal_handler},  //标识5
        {SIGCHLD,"SIGCHLD",ngx_signal_handler}, //子进程退出时，父进程会收到这个信号--标识17
        {SIGQUIT,"SIGQUIT",ngx_signal_handler},  //标识3
        {SIGIO,"SIGIO",ngx_signal_handler},      //指示一个异步I/O事件【通用异步I/O事件】
        {SIGSYS,"SIGSYS,SIGIGN", nullptr},       //我们想忽略这个信号,SIGSYS表示收到了一个无效的系统调用，如果我们不忽略，进程会被系统杀死，标识31
        {0, nullptr, nullptr}                    //信号对应的数字至少是1，所以可以用0作为一个特殊标记

};

//初始化信号的函数，用于注册信号处理程序
//0成功 -1失败
int ngx_init_signals()
{
    ngx_signal_t *sig;
    struct sigaction sa;
    for(sig = signals; sig->signo != 0 ;sig++){
        memset(&sa,0,sizeof (struct sigaction));
        if(sig->handler){
            sa.sa_sigaction = sig->handler;
            sa.sa_flags = SA_SIGINFO;  //sa_flags:int型 指定信号处理函数，设置该标记(SA_SIGINFO),就标识信号附带参数可以被传递到信号处理函数中，就是让sa.sa_sigaction指定的信号处理函数生效，
        }else{
            sa.sa_handler = SIG_IGN;   //sa_handler:这个标记SIG_IGN给到sa_handler成员，表示忽略信号的处理程序，否则操作系统的缺省信号处理程序很可能把这个进程杀掉;
                                       //其实sa_handler和sa_sigaction都是一个函数指针用来表示信号处理程序，只不过这两个函数指针他们参数不一样，sa_sigaction带的参数多，信息量大
                                       //而sa_handler带的参数少，信息量少；如果你想用sa_sigaction，那么就需要把sa_flags设置为SA_SIGINFO
        }
        sigemptyset(&sa.sa_mask);   //如咱们处理某个信号比如SIGUSR1信号时不希望收到SIGUSR2信号，那咱们就可以用诸如sigaddset(&sa.sa_mask,SIGUSR2);这样的语句针对信号为SIGUSR1时做处理，；
        //这里.sa_mask是个信号集（描述信号的集合），用于表示要阻塞的信号，sigemptyset()这个函数咱们在第三章第五节讲过：把信号集中的所有信号清0，本意就是不准备阻塞任何信号

        //设置信号处理动作（信号处理函数），说白了这里就是让这个信号来了调用我的处理程序，有个老的同类函数叫signal，不过signal这个函数被认为是不可靠信号语义，不建议使用，大家统一用sigaction
        if(auto ret =  sigaction(sig->signo,&sa, nullptr); ret == -1){
            SPDLOG_LOGGER_ERROR(spdlog::get("nginx"),"sigaction({}) fialed",sig->signame);
            return -1;
        }else{
            SPDLOG_LOGGER_INFO(spdlog::get("nginx"),"sigaction({}) successd!",sig->signame);
        }

    }
    return -1;
}

//信号处理函数
static void ngx_signal_handler(int signo, siginfo_t *siginfo, void *ucontext)
{
    printf("来信号了%d\n",signo);
}
