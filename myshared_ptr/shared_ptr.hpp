//
// Created by Administrator on 2024-03-25.
//

#ifndef MYSHARED_PTR_SHARED_PTR_HPP
#define MYSHARED_PTR_SHARED_PTR_HPP
#include <atomic>

namespace xqq{

    template<typename T>
    class default_deleter{
    public:
        void operator[](T* ptr){
            delete ptr;
        }
    };

    template<typename T, typename deleter = default_deleter<T>>
    class shared_ptr{
    private:
        T* ptr_ = nullptr;
        std::atomic<int>* count_ = nullptr;
        //默认构造 裸指针构造 拷贝构造（左值，右值） 拷贝复制（左值 右值） 析构
        //get swap user_count reset unique shared_ptr<T> make_shared()
        //重载 -> *
        deleter d;
    public:
        shared_ptr() = default;
        shared_ptr(T* ptr);
        shared_ptr(const shared_ptr& lsh);
        shared_ptr(shared_ptr&& rsh);
        shared_ptr& operator=(const shared_ptr& lsh);
        shared_ptr& operator=(shared_ptr&& rsh);
        ~shared_ptr();
        T* get() const;
        uint32_t user_count() const;
        bool unique() const;
        void swap(shared_ptr&& sh);
        void reset(T * ptr);
        T* operator->();
        T& operator*();
    };

    template<typename T,typename deleter>
    shared_ptr<T,deleter>::shared_ptr(T *ptr) :ptr_(ptr), count_(new std::atomic<int>(1)) {

    }
    template<typename T,typename deleter>
    shared_ptr<T,deleter>::shared_ptr(const shared_ptr& lsh){
        ptr_ = lsh.ptr_;
        count_ = lsh.count_;
        (*count_)++;
    }

    template<typename T,typename deleter>
    shared_ptr<T,deleter>::shared_ptr(shared_ptr&& rsh){
        std::swap(ptr_,rsh.ptr_);
        std::swap(count_, rsh.count_);
    }

    template<typename T,typename deleter>
    shared_ptr<T, deleter> &shared_ptr<T, deleter>::operator=(shared_ptr<T, deleter> &&rsh) {
        if(this != &rsh){
            if(ptr_ != nullptr){
                int expected = *count_;
                while(!count_->compare_exchange_weak(expected, expected - 1)){}
                if(expected == 1){
                    delete ptr_;
                    delete count_;
                    ptr_ = nullptr;
                    count_ = nullptr;
                }
                std::swap(ptr_,rsh.ptr_);
                std::swap(count_, rsh.count_);
            }
        }
    }

    template<typename T, typename deleter>
    shared_ptr<T, deleter> &shared_ptr<T, deleter>::operator=(const shared_ptr &lsh) {
        if(this != &lsh){
            if(ptr_ != nullptr){
                int expected = *count_;
                while(!count_->compare_exchange_weak(expected, expected - 1)){
                }
                if(expected == 1){
                    delete ptr_;
                    delete count_;
                    ptr_ = nullptr;
                    count_ = nullptr;
                }
            }
            ptr_ = lsh.ptr_;
            count_ = lsh.count_;
            (*count_)++;
        }
        return (*this);
    }

    template<typename T, typename deleter>
    shared_ptr<T, deleter>::~shared_ptr() {
        int expected = *count_;
        while(count_->compare_exchange_weak(expected,expected-1)){}
        if(expected == 1){
            delete ptr_;
            delete count_;
            ptr_ = nullptr;
            count_ = nullptr;
        }
    }

    template<typename T, typename deleter>
    T *shared_ptr<T, deleter>::get() const {
        return ptr_;
    }

    template<typename T, typename deleter>
    uint32_t shared_ptr<T, deleter>::user_count() const {
        return *count_;
    }

    template<typename T, typename deleter>
    bool shared_ptr<T, deleter>::unique() const {
        return (*count_) == 1;
    }

    template<typename T, typename deleter>
    void shared_ptr<T, deleter>::swap(shared_ptr &&sh) {
        std::swap(ptr_,sh.ptr);
        std::swap(count_,sh.count_);
    }

    template<typename T, typename deleter>
    void shared_ptr<T, deleter>::reset(T *ptr) {
        if(ptr != nullptr || ptr_ != ptr){
            shared_ptr<T,deleter> sp(std::move(*this));  //所有权交给sp 当前对象没有所有权了
            ptr_ = ptr;
            count_ = new std::atomic<int>(1);
        }
    }

    template<typename T, typename deleter>
    T *shared_ptr<T, deleter>::operator->() {
        return ptr_;
    }

    template<typename T, typename deleter>
    T &shared_ptr<T, deleter>::operator*() {
        return *ptr_;
    }

}

#endif //MYSHARED_PTR_SHARED_PTR_HPP
