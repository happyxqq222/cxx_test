//
// Created by Administrator on 2024-03-24.
//

#include "Timestamp.h"
#include "Logger.h"
#include <chrono>
#include <iomanip>
#include <iostream>

using namespace std::chrono;

using namespace std;

Timestamp::Timestamp() : currentTimepoint_(0ns) {
}

Timestamp::Timestamp(int64_t nanoSecondsSinceEpoch) : currentTimepoint_(nanoseconds (nanoSecondsSinceEpoch)) {
    Logger::getLogger()->info("{}",currentTimepoint_.time_since_epoch().count());
}

Timestamp::Timestamp(const system_clock::time_point &timePoint) : currentTimepoint_(timePoint) {
}

Timestamp Timestamp::now() {
    return Timestamp(std::chrono::system_clock::now());
}

std::string Timestamp::toString() const {
    auto t = chrono::system_clock::to_time_t(currentTimepoint_);
    stringstream ss;
    tm* tm_time = localtime(&t);
    auto putTime =  put_time(tm_time,"%Y-%m-%d %H:%M:%S");
    ss << putTime;
    string timeStr = ss.str();
    return timeStr;
}
