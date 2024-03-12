//
// Created by Administrator on 2024-03-07.
//
#include <stdio.h>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO

#include <iostream>
#include "ngx_func.h"  
#include "ngx_signal.h"
#include "app/ngx_c_conf.h"
#include <thread>
#include <chrono>
#include "unistd.h"
#include "app/Environment.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/async.h"
#include "signal/ngx_signal.h"
#include "NginxProcess.h"


using namespace std;
pid_t ngx_pid;               //当前进程的pid
pid_t ngx_parent;            //父进程的pid

//shared_ptr<spdlog::async_logger> logger;
void initlog()
{    using namespace std::literals;
    spdlog::init_thread_pool(8192, 1);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/mylog", 1024*1024*10, 3);
    std::vector<spdlog::sink_ptr> sinks {stdout_sink, rotating_sink};
    auto logger = std::make_shared<spdlog::async_logger>("nginx", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    logger->set_pattern("[%@] [%l] [%Y:%H:%M:%S %z] [thread %t] %v");
    spdlog::register_logger(logger);
}

int main(int argc, char * argv[])
{
    initlog();

    Environment::getInstance(argc,argv);

    ngx_init_signals();

    CConfig * cConfig = CConfig::getInstance();
    if(cConfig != nullptr && cConfig->load("nginx.conf") == false){
        printf("error\n");
        exit(1);
    }

    NginxProcess::getInstance();


    for(;;){
        this_thread::sleep_for(chrono::seconds(1));
    }
    return 0;
}