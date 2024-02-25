#include <iostream>
#include <vector>
#include "SgiPoolAllocator.h"

int main() {
    myallocator<int> m;
    std::vector<int,myallocator<int>> v;
    for(int i =0;i<10;i++){
        v.push_back(i+1);
    }

    for(auto& a : v){
        std::cout << a << std::endl;
    }

    return 0;
}
