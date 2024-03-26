//
// Created by Administrator on 2024-03-24.
//

#include "Logger.h"

using namespace std;


std::shared_ptr<spdlog::async_logger> Logger::logger_;

bool Logger::isInitialize = false;

void Logger::initLogger() {
    if (isInitialize) {
        return;
    }
    spdlog::init_thread_pool(8192, 1);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("log.txt", 1024 * 1024 * 10, 3);
    std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink};
    logger_ = std::make_shared<spdlog::async_logger>("log", sinks.begin(), sinks.end(), spdlog::thread_pool());
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->set_pattern("[%Y-%m-%d %T.%e][tid:%t][%l][%^---%L---%$] %v");
    logger_->set_formatter(std::move(formatter));
    spdlog::register_logger(logger_);
    isInitialize = true;
}

spdlog::async_logger* Logger::getLogger() {
    return logger_.get();
}
