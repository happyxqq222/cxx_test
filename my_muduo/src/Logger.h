//
// Created by Administrator on 2024-03-24.
//

#ifndef MY_MUDUO_LOGGER_H
#define MY_MUDUO_LOGGER_H
#include "noncopyable.h"
#include <memory>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/async.h"
#include "spdlog/pattern_formatter.h"

class Logger : noncopyable {
public:
    static void initLogger();
    static std::shared_ptr<spdlog::async_logger> getLogger();
private:
    static std::shared_ptr<spdlog::async_logger> logger_;
    static bool isInitialize;
};


#endif //MY_MUDUO_LOGGER_H
