//
// Created by Administrator on 2024-04-25.
//

#ifndef THREADPOOL_TEST_TIMEUNIT_H
#define THREADPOOL_TEST_TIMEUNIT_H

#include <variant>
#include <chrono>
#include <optional>

enum class Unit{
    HOURS,
    MINUTES,
    SECONDS,
    MILLISECONDS,
};

template<Unit unit = Unit::SECONDS>
class TimeUnit{
public:
    TimeUnit(int64_t time){
        if constexpr (unit == Unit::HOURS){
            time_ = std::chrono::hours(time);
        }else if constexpr (unit == Unit::MINUTES){
            time_ = std::chrono::minutes(time);
        }else if constexpr (unit == Unit::SECONDS){
            time_ = std::chrono::seconds(time);
        }else if constexpr (unit == Unit::MILLISECONDS){
            time_ = std::chrono::milliseconds (time);
        }
    }

    TimeUnit(){
        if constexpr (unit == Unit::HOURS){
            time_ = std::chrono::hours(0);
        }else if constexpr (unit == Unit::MINUTES){
            time_ = std::chrono::minutes(0);
        }else if constexpr (unit == Unit::SECONDS){
            time_ = std::chrono::seconds(0);
        }else if constexpr (unit == Unit::MILLISECONDS){
            time_ = std::chrono::milliseconds (0);
        }
    }

    std::optional<std::chrono::milliseconds> getTimeMillDuration(){
        if constexpr (unit == Unit::HOURS){
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::get<std::chrono::hours>(time_));
        }else if constexpr (unit == Unit::MINUTES){
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::get<std::chrono::minutes>(time_));
        }else if constexpr (unit == Unit::SECONDS){
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::get<std::chrono::seconds>(time_));
        }else if constexpr (unit == Unit::MILLISECONDS){
            return std::get<std::chrono::milliseconds>(time_);
        }else{
            return std::nullopt;
        }

    }

private:
    using TimeVariant = std::variant<std::chrono::hours,std::chrono::minutes,std::chrono::seconds,std::chrono::milliseconds>;
    TimeVariant time_;
};


#endif //THREADPOOL_TEST_TIMEUNIT_H
