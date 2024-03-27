#include <iostream>
#include "Logger.h"
#include <chrono>
#include "Timestamp.h"
#include "InetAddress.h"
#include <boost/type_index.hpp>
#include <sys/epoll.h>
#include "CurrentThread.h"

#include "EpollPoller.h"

using namespace std;
using namespace std::chrono;
using namespace boost::typeindex;

__thread int l = 0;

int main() {

    Logger::initLogger();
    return 0;
}
