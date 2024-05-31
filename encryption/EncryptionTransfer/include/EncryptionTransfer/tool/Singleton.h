#pragma once
#include <memory>
#include <mutex>

#include <boost/core/noncopyable.hpp>

template<typename T>
class Singleton : boost::noncopyable {

public:
    static std::shared_ptr<T> getInstance() {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&](){
            _instance = std::shared_ptr<T>(new T);
        });
        return _instance;
    }

    ~Singleton() = default;

protected:
    Singleton() = default;
    static std::shared_ptr<T> _instance;
};

template<typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;