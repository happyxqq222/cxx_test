//
// Created by Administrator on 2024-03-24.
//

#ifndef MY_MUDUO_TIMESTAMP_H
#define MY_MUDUO_TIMESTAMP_H


#include <cstdint>
#include <string>
#include <chrono>

class Timestamp {

public:
    Timestamp();
    explicit Timestamp(int64_t nanoSecondsSinceEpoch);
    explicit Timestamp(const std::chrono::system_clock::time_point & timePoint);
    static Timestamp now();
    std::string toString() const;

private:
    std::chrono::system_clock::time_point currentTimepoint_;
};


#endif //MY_MUDUO_TIMESTAMP_H
