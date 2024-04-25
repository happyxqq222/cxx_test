//
// Created by Administrator on 2024-04-24.
//

#ifndef THREADPOOL_TEST_ANY_H
#define THREADPOOL_TEST_ANY_H

#include <memory>
#include <boost/type_index.hpp>
#include <iostream>
#include <optional>

namespace pool {
    class Any;
}

using namespace boost::typeindex;

class Any {

public:
    Any() :base_(nullptr){
    }

    bool isEmpty(){
        return base_ == nullptr;
    }

    template<typename Up,typename _Up2 = std::remove_cv_t<std::remove_reference_t<Up>>>
    using not_same = typename std::enable_if<!std::is_same_v<Any,_Up2>>::type;

    template<typename T,typename = not_same<T>>
    Any(T &&data){
        if constexpr (std::is_convertible_v<T,std::string>){
            base_ = std::make_unique<Derived<std::string>>(std::forward<T>(data));
        }else{
            base_ = std::make_unique<Derived<T>>(std::forward<T>(data));
        }
    }

    template<typename T>
    std::optional<std::reference_wrapper<T>> cast(){
        Derived<T> * pD = dynamic_cast<Derived<T>*>(base_.get());
        if(pD == nullptr){
            return std::nullopt;
        }
        return pD->data_;
    }
    Any(Any&& any) : base_(std::move(any.base_)) {

    };
    Any& operator=(Any&& other) {
        if(this == &other){
            return *this;
        }
        this->base_ = std::move(other.base_);
        return *this;
    };
    Any(const Any&) = delete;
    Any& operator=(const Any&) = delete;

private:
    class Base {
    public:
        virtual ~Base() = default;
    };

    template<typename T>
    class Derived : public Base {

    public:
        Derived(T &&data) : data_(std::forward<T>(data)) {
        }

        T data_;
    };

private:
    std::unique_ptr<Base> base_;
};

#endif //THREADPOOL_TEST_ANY_H
