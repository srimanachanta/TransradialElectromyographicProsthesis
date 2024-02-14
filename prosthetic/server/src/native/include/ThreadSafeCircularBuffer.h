#pragma once

#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>

template <typename T> class ThreadSafeCircularBuffer {
    boost::circular_buffer<T> buf;
    boost::mutex lock;

public:
    explicit ThreadSafeCircularBuffer(size_t size) : buf(size) {}

    void push_back(T item) {
        lock.lock();
        buf.push_back(item);
        lock.unlock();
    }

    size_t get_size() {
        lock.lock();
        auto size =  buf.size();
        lock.unlock();

        return size;
    }

    bool is_full() {
        lock.lock();
        auto f = buf.size() == buf.capacity();
        lock.unlock();
        return f;
    }

    std::vector<T> get_buffer() {
        lock.lock();
        std::vector<T> container;
        container.reserve(buf.size());
        for(const auto& item : buf) {
            container.emplace_back(item);
        }
        lock.unlock();

        return container;
    }
};