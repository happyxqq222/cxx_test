//
// Created by Administrator on 1/11/2024.
//

#ifndef CASDQUEUE_CASDQUEUE_H
#define CASDQUEUE_CASDQUEUE_H

#include <atomic>

template<typename T>
struct Node {
    T data;
    Node *prev;
    Node *next;
};

template<typename T>
class CasDqueue {
    typedef T value_type;
private:
    Node<T> *tail;
    std::atomic<Node<T>*> head;
public:
    void push_back(const T &value);

    void push_back(T &&value);

    value_type &pop_back();

};


#endif //CASDQUEUE_CASDQUEUE_H
