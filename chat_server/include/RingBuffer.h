//
// Created by Administrator on 2024-04-13.
//

#ifndef CHAT_SERVER_RINGBUFFER_H
#define CHAT_SERVER_RINGBUFFER_H

#include <atomic>
#include <memory>

/**
 * 无锁环形队列
 */
template<typename Ty, size_t Capacity, typename Alloc = std::allocator<Ty>>
class RingBuffer {
public:
    RingBuffer()
            : _alloc(),_maxSize(Capacity + 1), _head(0), _tail(0), _tailUpdate(0), _data(_alloc.allocate(Capacity+1)) {
    }

    RingBuffer(const RingBuffer &other) = delete;

    RingBuffer &operator=(const RingBuffer &other) = delete;

    ~RingBuffer();

    template<typename... Args>
    bool emplace_back(Args &&... args);

    std::shared_ptr<Ty> pop_back();

private:
    Alloc _alloc;
    std::atomic<size_t> _head;
    std::atomic<size_t> _tail;
    std::atomic<size_t> _tailUpdate;
    Ty *_data;
    size_t _maxSize;
};

template<typename Ty, size_t Capacity, typename Alloc>
template<typename... Args>
bool RingBuffer<Ty, Capacity, Alloc>::emplace_back(Args &&... args) {
    size_t tail;
    do {
        tail = _tail.load(std::memory_order_relaxed);
        if ((tail + 1) % _maxSize == _head.load(std::memory_order_acquire)) {
            std::cout << "full" << std::endl;
            return false;
        }
    } while (!_tail.compare_exchange_strong(tail, (tail + 1) % _maxSize, std::memory_order_release,
                                           std::memory_order_relaxed));
    _alloc.construct(_data + _tail, std::forward<Args>(args)...);

    size_t tailUpdate;
    do {
        tailUpdate = tail;
    } while (!_tailUpdate.compare_exchange_strong
            (tailUpdate, (tailUpdate + 1) % _maxSize,
             std::memory_order_release, std::memory_order_relaxed));
    return true;
}

template<typename Ty, size_t Capacity, typename Alloc>
std::shared_ptr<Ty> RingBuffer<Ty, Capacity, Alloc>::pop_back() {
    size_t head;
    std::shared_ptr<Ty> tyPtr;
    do {
        head = _head.load(std::memory_order_acquire);
        //判断头部和尾部指针是否重合，如果重合则队列为空
        if (head == _tail.load(std::memory_order_acquire)) {
//            std::cout << "empty" << std::endl;
            return nullptr;
        }
        //如果此时要读取的数据和tail_update是否一致，如果一致说明尾部数据未更新完
        if (head == _tailUpdate.load(std::memory_order_acquire)) {
            return nullptr;
        }
        tyPtr = std::make_shared<Ty>(_data[head]);
    } while (!_head.compare_exchange_strong(head, (head + 1) % _maxSize,
                                            std::memory_order_release,
                                            std::memory_order_relaxed));
    return tyPtr;
}


template<typename Ty, size_t Capacity, typename Alloc>
RingBuffer<Ty, Capacity, Alloc>::~RingBuffer() {
    while(_head != _tail){
        _alloc.destroy(_data+_head);
        _head = (++_head) % _maxSize;
    }
    _alloc.deallocate(_data,_maxSize);
}


#endif //CHAT_SERVER_RINGBUFFER_H
