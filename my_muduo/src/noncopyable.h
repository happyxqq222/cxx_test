//
// Created by Administrator on 2024-03-24.
//

#ifndef MY_MUDUO_NONCOPYABLE_H
#define MY_MUDUO_NONCOPYABLE_H


class noncopyable {
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};


#endif //MY_MUDUO_NONCOPYABLE_H
