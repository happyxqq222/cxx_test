//
// Created by Administrator on 2024/6/2.
//

#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <boost/core/noncopyable.hpp>
#include <deque>


namespace xqq {
    template<typename  T>
    class BoundedBlockingQueue : public boost::noncopyable {
    public:
        using queue_type = std::deque<T>;

        explicit BoundedBlockingQueue(int size) : mutex_(), queue_(), size_(size) {
            assert(size > 0);
        }

        void push(const T &x) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (queue_.size() == size_) {
                notFull_.wait(lock,[&] {
                    return queue_.size() < size_.load(std::memory_order_acquire);
                });
                return;
            }
            queue_.push_back(x);
            size_.fetch_add(1,std::memory_order_release);
            notEmpty_.notify_one();
        }

        void push(T &&t) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (queue_.size() == size_.load(std::memory_order_acquire)) {
                notFull_.wait(lock,[&] {
                    return queue_.size() < size_.load(std::memory_order_acquire);
                });
                return;
            }
            queue_.push_back(std::move(t));
            size_.fetch_add(1,std::memory_order_release);
            notEmpty_.notify_one();
        }

        void take(T& t) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (queue_.empty()) {
                notEmpty_.wait(lock, [this] {
                    return !this->queue_.empty();
                });
            }
            assert(!queue_.empty());
            t = std::move(queue_.front());
            queue_.pop_front();
            notFull_.notify_one();
        }

        bool empty() const {
            std::unique_lock<std::mutex> lock(mutex_);
            return queue_.empty();
        }

        bool full() const {
            std::unique_lock<std::mutex> lock(mutex_);
            return queue_.size() == size_.load(std::memory_order_acquire);
        }



    private:
        alignas(64) std::atomic_int size_;
        mutable  std::mutex mutex_;
        std::condition_variable notEmpty_;
        std::condition_variable notFull_;
        queue_type queue_;
    };
}


#endif //BLOCKINGQUEUE_H
