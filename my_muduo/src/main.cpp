#include <iostream>
#include "Logger.h"
#include <chrono>
#include "Timestamp.h"
#include "InetAddress.h"
#include <boost/type_index.hpp>
#include <sys/epoll.h>

using namespace std;
using namespace std::chrono;
using namespace boost::typeindex;


int main() {
    Logger::initLogger();
    vector<int> a(10);
    cout << a.size() << " " << a.capacity() << endl;

    return 0;
}
