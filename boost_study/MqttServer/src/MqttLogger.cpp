//
// Created by xqq on 24-6-6.
//

#include "MqttLogger.h"
#include <memory>
#include <vector>
#include "MqttConfig.h"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/spdlog.h"

using namespace std;
using namespace spdlog;

bool MqttLogger::init(const std::string &logFile, uint64_t maxRotateSize, uint64_t maxRotateCount) {

    try{
        spdlog::init_thread_pool(MqttConfig::getInstance()->thread_pool_qsize(),
                                 MqttConfig::getInstance()->thread_count());
        auto console_sink = make_shared<sinks::stdout_color_sink_mt>();
        auto file_sink = make_shared<sinks::rotating_file_sink_mt>(
                logFile,maxRotateSize,maxRotateCount);
        vector<sink_ptr> sinkPtrs;
        sinkPtrs.push_back(console_sink);
        sinkPtrs.push_back(file_sink);
        spdlog::set_default_logger(
                std::make_shared<spdlog::async_logger>("debug_logger",sinks_init_list({console_sink,file_sink}),
                           spdlog::thread_pool(),
                           spdlog::async_overflow_policy::block)
                           );
        spdlog::set_level(spdlog::level::info);
        spdlog::set_pattern("[%Y-%m-%d %T.%f] [%^%l%$] [%s:%#] %v");

    }catch (const spdlog_ex& ex) {
        std::printf("MqttLogger init failed");
        return false;
    }
    return true;
}
