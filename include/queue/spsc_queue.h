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

    std::atomic<size_t> currentDepth{0};

    std::atomic<size_t> peakDepth{0};

public:

    SPSCQueue(size_t size)
        : buffer(size),
          capacity(size),
          head(0),
          tail(0) {}

    bool push(const T& item) {

        size_t currentTail =
            tail.load(std::memory_order_relaxed);

        size_t nextTail =
            (currentTail + 1) % capacity;

        if(
            nextTail ==
            head.load(
                std::memory_order_acquire
            )
        ) {
            return false;
        }

        buffer[currentTail] = item;

        tail.store(
            nextTail,
            std::memory_order_release
        );

        size_t depth =
            ++currentDepth;

        size_t peak =
            peakDepth.load();

        while(
            depth > peak &&
            !peakDepth.compare_exchange_weak(
                peak,
                depth
            )
        ) {
        }

        return true;
    }

    bool pop(T& item) {

        size_t currentHead =
            head.load(
                std::memory_order_relaxed
            );

        if(
            currentHead ==
            tail.load(
                std::memory_order_acquire
            )
        ) {
            return false;
        }

        item =
            buffer[currentHead];

        head.store(
            (currentHead + 1) % capacity,
            std::memory_order_release
        );

        --currentDepth;

        return true;
    }

    size_t getCurrentDepth() const {

        return currentDepth.load();
    }

    size_t getPeakDepth() const {

        return peakDepth.load();
    }
};