//
// Created by Administrator on 2024-03-12.
//

#include "include/NginxProcess.h"
#include "signal.h"
#include "spdlog/spdlog.h"
#include "app/ngx_c_conf.h"
#include "sys/prctl.h"
#include "ngx_global.h"
#include <thread>

NginxProcess::NginxProcess() {
    masterProcessCycle();
}

NginxProcess* NginxProcess::getInstance() {
    static NginxProcess processInit;
    return &processInit;
}

void NginxProcess::masterProcessCycle() {
    ngx_pid = getpid();
    sigset_t set;  //信号集
    sigemptyset(&set);
    sigaddset(&set,SIGCHLD);   //子进程状态改变
    sigaddset(&set,SIGALRM);   //定时器超时
    sigaddset(&set,SIGIO);     //异步I/O
    sigaddset(&set,SIGINT);    //终端终端符
    sigaddset(&set,SIGHUP);    //连接断开
    sigaddset(&set,SIGUSR1);   //用户定义信号
    sigaddset(&set,SIGUSR2);    //用户定义信号
    sigaddset(&set,SIGWINCH);   //终端窗口大小改变
    sigaddset(&set,SIGTERM);   //终止
    sigaddset(&set,SIGQUIT);   //终端退出符

    if(sigprocmask(SIG_BLOCK,&set,NULL) == -1){
        SPDLOG_LOGGER_ERROR(spdlog::get("nginx"),"sigprocmask filed!");
    }

    //设置主进程标题
    prctl(PR_SET_NAME,"master process",0,0,0);
    size_t size;
    int i ;
    CConfig *pConfig = CConfig::getInstance();
    auto workerProcessNum = pConfig->getIntDefault("WorkerProcesses", 1);
    startWorkerProcess(*workerProcessNum);
    sigemptyset(&set);//信号屏蔽字为空，表示不屏蔽任何信号

    for(;;){
        //a)根据给定的参数设置新的mask 并 阻塞当前进程【因为是个空集，所以不阻塞任何信号】
        //b)此时，一旦收到信号，便恢复原先的信号屏蔽【我们原来的mask在上边设置的，阻塞了多达10个信号，从而保证我下边的执行流程不会再次被其他信号截断】
        //c)调用该信号对应的信号处理函数
        //d)信号处理函数返回后，sigsuspend返回，使程序流程继续往下走
        sigsuspend(&set);
    }
    return;

}

void NginxProcess::startWorkerProcess(int workerPrcessNum) {
    for(int i = 0; i < workerPrcessNum; i++){
        spawnProcess(i,"worker process");
    }
    return;
}

int NginxProcess::spawnProcess(int index, std::string_view title) {
    pid_t  pid;
    pid = fork();
    switch(pid){
        case -1:
            SPDLOG_LOGGER_ERROR(spdlog::get("nginx"),"fork index={} failed processname={}",index,title);
            return -1;
        case 0:
            ngx_parent = ngx_pid;
            ngx_pid = getpid();
            prctl(PR_SET_NAME,title.data(),0,0,0);
            workerProcessCycle();
            break;
        default:   //父进程分支
            break;
    }
    return pid;
}

void NginxProcess::workerProcessCycle() {
    sigset_t set;
    sigemptyset(&set);
    if(sigprocmask(SIG_SETMASK,&set,NULL) == -1){
        SPDLOG_LOGGER_ERROR(spdlog::get("nginx"),"setprocmask failured pid={}",ngx_pid);
    }

    for(;;){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
