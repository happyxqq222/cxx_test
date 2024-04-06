//
// Created by Administrator on 2024-04-07.
//

#ifndef CHAT_SERVER_DISPATCH_H
#define CHAT_SERVER_DISPATCH_H
#include <iostream>

#include <mutex>
#include <memory>
#include <functional>

using namespace std;

template<typename Dispatcher,class Callable,class ...Args>
class DispatcherTemplate;

class Dispatcher{

    void dispatch(){
        cout << "dispatcher dispatch over";
    }
    template<typename Dispatcher,class Callable,class ...Args>
    friend class DispatcherTemplate;

public:
    template<class Callable,class ...Args>
    DispatcherTemplate<Dispatcher,Callable,Args...> handle(Callable&& callable,Args&& ...args){
        return DispatcherTemplate<Dispatcher,Callable,Args...>(this,std::forward<Callable>(callable),std::forward<Args>(args)...);
    }

};

template<typename Dispatcher,class Callable,class ...Args>
class DispatcherTemplate{

    using Tuple = std::tuple<decay_t<Args>...>;

    Callable f_;

    Dispatcher* prevDispatch_;
    std::unique_ptr<Tuple> argsTuple;

    constexpr static size_t indices = sizeof...(Args);

public:
    DispatcherTemplate(Dispatcher *preDispatcher, Callable &&f, Args&&... args)
            : prevDispatch_(preDispatcher),f_(std::forward<Callable>(f)){
        argsTuple = std::make_unique<Tuple>(std::forward<Args>(args)...);
    }

    DispatcherTemplate(const Dispatcher&) = delete;
    DispatcherTemplate& operator=(const Dispatcher&) = delete;

    DispatcherTemplate(DispatcherTemplate &&other)
            : prevDispatch_(other.prevDispatch_),
              argsTuple(std::move(argsTuple)),
              f_(std::move(other.f_)) {
        other.prevDispatch_ = nullptr;
    }

    template<size_t... N>
    void invokeCallable(index_sequence<N...>){
        f_(std::get<N>(*argsTuple)...);
    }

    template<class OtherCallable,class ...OtherArgs>
    DispatcherTemplate<DispatcherTemplate,OtherCallable,OtherArgs...> handle(OtherCallable&& callable,OtherArgs&&... args){
        return DispatcherTemplate<DispatcherTemplate,OtherCallable,OtherArgs...>(this,std::forward<OtherCallable>(callable),std::forward<OtherArgs>(args)...);
    }

    void start(){
        dispatch();
    }

    void dispatch(){
        invokeCallable(make_index_sequence<indices>{});
        prevDispatch_->dispatch();
    }
};
#endif //CHAT_SERVER_DISPATCH_H
