//
// Created by Administrator on 12/31/2023.
//
#include <iostream>
#include <atomic>

using namespace std;


int main() {
    int j = 33;
    cout << j << endl;
    int value = 10;
    atomic<int> ai = 10;
    ai.fetch_add(1);
    int expectValue = 10;
    atomic_compare_exchange_strong(&ai, &expectValue, 11);
    cout << ai << endl;
    return 1;
}