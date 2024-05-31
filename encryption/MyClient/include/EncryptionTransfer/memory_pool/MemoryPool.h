//
// Created by Administrator on 2024-05-17.
//

#ifndef ENCRYPTIONTRANSFER_MEMORYPOOL_H
#define ENCRYPTIONTRANSFER_MEMORYPOOL_H


#include <cstdint>
#include <memory>

struct pool;

struct pool_data {
    uint8_t *last;
    uint8_t *end;
    pool *next;
    uint8_t failed;
};

struct pool{
    pool_data d;
    size_t max;
    pool *current;
    void* chain;
};

class MemoryPool {

private:
};







#endif //ENCRYPTIONTRANSFER_MEMORYPOOL_H
