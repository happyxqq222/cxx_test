#include <iostream>
#include "Logger.h"
#include <chrono>
#include "Timestamp.h"
#include "InetAddress.h"
#include "CurrentThread.h"
#include "EventLoopThread.h"
#include "EventLoop.h"
#include "EpollPoller.h"

#include <boost/type_index.hpp>
#include <sys/epoll.h>
#include <functional>

using namespace std;
using namespace std::chrono;
using namespace boost::typeindex;

__thread int l = 0;

template<typename T>
void print1(T&& t){
    std::cout << type_id_with_cvr<T>().pretty_name() << endl;
}


int main() {
    Logger::initLogger();

    EventLoopThread::ThreadInitCallback f = [](EventLoop* ev)->void{

    };
    char ch[1024] = "sdfsdaf";
    std::string str = "af";
    EventLoopThread t{f , ch};


    return 0;
}
