//
// Created by Administrator on 2024-04-24.
//

#ifndef THREADPOOL_TEST_NONCOPYABLE_H
#define THREADPOOL_TEST_NONCOPYABLE_H

class noncopyable{
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

#endif //THREADPOOL_TEST_NONCOPYABLE_H
