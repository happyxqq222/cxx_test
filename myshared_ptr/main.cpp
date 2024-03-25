#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <memory>
#include "shared_ptr.hpp"

using namespace std;

template<typename T>
class lock_free_stack
{
private:
    struct node{
        T data;
        node* next;
        node(T const& data_) : data(data_){}
    };
    std::atomic<node*> head;
public:
    void push(T const& data){

        node* const newNode = new node(data);
        cout << "init thread_id:"<< this_thread::get_id() << ",head:" << head.load() << ",newNode:" << newNode << endl;
        newNode->next = head.load(); //如果head更新了，这条语句要重来一遍
        if(this_thread::get_id()  == thread::id(1)){
            this_thread::sleep_for(6s);
        }
        cout << "compare thread_id:"<< this_thread::get_id() << ",head:" << head.load() << ",newNode:" << newNode << endl;
        while(!head.compare_exchange_weak(newNode->next,newNode)){
            cout << "thread_id:" << this_thread::get_id()  << " newNode->next:" << newNode->next << " head:" << head.load() << endl;
        }
    }

};

class A{
public:
    ~A(){
        cout << "~A()" << endl;
    }
};

int main() {
    xqq::shared_ptr<A> sp1(new A);
    cout << sp1.user_count() << endl;
    xqq::shared_ptr<A> sp2 = sp1;
    cout << sp2.user_count() << endl;

    return 0;
}
