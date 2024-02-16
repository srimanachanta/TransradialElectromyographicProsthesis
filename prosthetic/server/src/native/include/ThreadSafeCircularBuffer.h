#pragma once

#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>

template <typename T> class ThreadSafeCircularBuffer {
    boost::circular_buffer<T> buf;
    boost::mutex lock;

public:
    explicit ThreadSafeCircularBuffer(size_t size) : buf(size) {}

    void push_back(T& item) {
        lock.lock();
        buf.push_back(item);
        lock.unlock();
    }

    bool is_full() {
        lock.lock();
        auto f = buf.size() == buf.capacity();
        lock.unlock();

        return f;
    }

    boost::circular_buffer<T> get_buffer() {
        lock.lock();
        auto container = buf;
        lock.unlock();

        return container;
    }
};
