#include <iostream>
#include "CasDqueue.h"
#include "atomic"
#include <vector>
#include <thread>
#include <chrono>

using namespace std;

class T{
public:
   // long long p1,p2,p3,p4,p5,p6,p7,p8;
    int x;
   // long long pp1,pp2,pp3,pp4,pp5,pp6,pp7;
};
T t1[1];
T t2[1];
class A{
public:
    T t1;
    T t2;
};
int main() {
    using namespace chrono;
    steady_clock::time_point start =  steady_clock::now();
    A a;
    thread t1([&](){
        for(int i =0;i<200000000;i++){
            a.t1.x = 1000;
        }
    });
    thread t;
    thread t2([&](){
        for(int i =0;i<2000000000;i++){
            a.t2.x = 2000;
        }
    });
    t1.join();
    t2.join();
    steady_clock::time_point end = steady_clock::now();
    duration<double> duration = duration_cast<std::chrono::duration<double>>(end - start);
    std::cout << "Elapsed time: " << duration.count() << " seconds\n";
    return 0;
}
