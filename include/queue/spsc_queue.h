#pragma once

#include <vector>
#include <atomic>

template<typename T>

class SPSCQueue {

private:
    std::vector<T> buffer;
    size_t capacity;

    std::atomic<size_t> head;
    std::atomic<size_t> tail;

public:
    SPSCQueue(size_t size) : buffer(size), capacity(size), head(0), tail(0) {}

    bool push(const T& item) {

        size_t currentTail = tail.load(std::memory_order_relaxed);
        size_t nextTail = (currentTail + 1) % capacity;

        if(nextTail == head.load(std::memory_order_acquire)) {
            return false; // full
        }

        buffer[currentTail] = item;
        tail.store(nextTail, std::memory_order_release);

        return true;
    }

    bool pop(T& item) {

        size_t currentHead = head.load(std::memory_order_relaxed);

        if(currentHead == tail.load(std::memory_order_acquire)) {
            return false; // empty
        }

        item = buffer[currentHead];
        head.store((currentHead + 1) % capacity, std::memory_order_release);

        return true;
    }
};