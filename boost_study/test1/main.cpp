#include <iostream>

using namespace std;

struct MyStruct {
    alignas(128) int a;
    alignas(128) double b;
    alignas(128) char c;
};

int main()
{
    int a;
    MyStruct s;
    s.a = 111;
    cout << s.a << endl;
    cout << "s的地址:" << &s << endl;
    cout << sizeof(MyStruct) << endl;
    return 0;
}
