#include <iostream>
#include "Logger.h"
#include <chrono>
#include "Timestamp.h"
#include "InetAddress.h"
#include <boost/type_index.hpp>

using namespace std;
using namespace std::chrono;
using namespace boost::typeindex;


int main() {
    Logger::initLogger();
    InetAddress inetAddress(88,"192.168.47.10");
    cout << inetAddress.toIpPort() << endl;
    return 0;
}
