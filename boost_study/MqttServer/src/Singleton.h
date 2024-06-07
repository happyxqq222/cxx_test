//
// Created by Administrator on 2024/6/3.
//

#ifndef SINGLETON_H
#define SINGLETON_H
#include <memory>
#include <iostream>
#include <mutex>

template<typename T>
class Singleton {
public:
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;
protected:
    Singleton() = default;
    static std::shared_ptr<T> instance_;

public:

    static std::shared_ptr<T> getInstance() {
        static std::once_flag sFlag;
        std::call_once(sFlag, [&]() {
           instance_ = std::shared_ptr<T>(new T);
        });
        return instance_;
    }

    void printAddress() {
        std::cout << instance_.get() << std::endl;
    }

    virtual ~Singleton() {
        std::cout << "this is singleton destruct " << std::endl;
    }
};

template<typename T>
std::shared_ptr<T> Singleton<T>::instance_ = nullptr;

#endif //SINGLETON_H
