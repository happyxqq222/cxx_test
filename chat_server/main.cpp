
#include "ChatServer.h"
#include "mysql_conn_pool/DBConnectionPool.h"
#include "mysql_conn_pool/DBConnection.h"
#include "include/Singleton.h"
#include "include/ArrayBlockQueue.h"
#include "include/RingBuffer.h"

#include <vector>
#include <iostream>
using namespace std;

mutex m;
vector<thread> v;


class Logic_System : public Singleton<Logic_System>
{
    friend class Singleton<Logic_System>;
public:
    ~Logic_System(){};
private:
    Logic_System() = default;
};

class A1
{
public:
    A1(int x,int y):_x(x),_y(y){}
    friend ostream& operator<<(ostream& out, A1& a1){
        cout << a1._x ;
        return out;
    }
private:
    int _x,_y;
};

int main()
{
    RingBuffer<A1,50000> ringBuffer;
    thread t1([&ringBuffer](){
        for(int i =0;i<10000;i++){
            ringBuffer.emplace_back(i,0);
            this_thread::sleep_for(100ms);
        }
    });
    thread t2([&ringBuffer](){
        while(true){
            auto res = ringBuffer.pop_back();
            if(res != nullptr){
                cout << *res << endl;
            }
        }
    });
    thread t3([&ringBuffer](){
        while(true){
            auto res = ringBuffer.pop_back();
            if(res != nullptr){
                cout << *res << endl;
            }
        }
    });
    t1.join();
    t2.join();
    t3.join();
    return 0;
}