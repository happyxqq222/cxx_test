//
// Created by Administrator on 2024-04-06.
//

#ifndef CHAT_SERVER_SINGLETON_H
#define CHAT_SERVER_SINGLETON_H

#include <memory>
#include <mutex>
#include <iostream>


template<typename T>
class Singleton;


template<typename T>
class Singleton{
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;
    static std::shared_ptr<T> _instance;
public:
    static std::shared_ptr<T> GetInstance(){
        static std::once_flag s_flag;
        std::call_once(s_flag,[&](){
           _instance = std::shared_ptr<T>(new T);
        });
        return _instance;
    }
    ~Singleton(){
        std::cout << "this is singleton destruct" << std::endl;
    }
};

template<typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;


#endif //CHAT_SERVER_SINGLETON_H
