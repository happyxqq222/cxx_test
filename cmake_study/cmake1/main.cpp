#include <iostream>
#include "makeTest.h"


#ifdef USER_MYMATH
#include "mysqrt.h"
#endif

using namespace std;
int main() {
    int a= 100;
    std::cout << "Hello, World!" << std::endl;
    std::cout << cmake1_VERSION_MAJOR << std::endl;
    std::cout << cmake1_VERSION_MINOR << std::endl;
    std::cout << STR_TEST << std::endl;
    std::cout << "你好" << endl;
    std::cout << "__cplusplus:" << __cplusplus << endl;
#ifdef USER_MYMATH
    cout << "sqrt(9)=" << mysqrt(9) << endl;
#endif
#ifdef EX3
    cout << "EX3 define" << endl;
#endif
    return 0;
}
