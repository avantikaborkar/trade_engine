#pragma once
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {

private:

    std::queue<T> queue;

    mutable std::mutex mutex;

    std::condition_variable cv;

    bool stopped = false;

    std::atomic<size_t> currentDepth{0};

std::atomic<size_t> peakDepth{0};
public:

    size_t size() const {

    std::lock_guard<std::mutex> lock(mutex);

        return queue.size();
    }


    void push(const T& item) {

        
            std::lock_guard<std::mutex> lock(mutex);

            queue.push(item);
            size_t depth = ++currentDepth;

                size_t peak = peakDepth.load();

                while(depth > peak &&
                    !peakDepth.compare_exchange_weak(
                        peak,
                        depth
                    ))
                {
                }

        cv.notify_one();
    }

    bool pop(T& item) {

        std::unique_lock<std::mutex> lock(mutex);

        cv.wait(lock, [this] {

            return !queue.empty() || stopped;
        });

        if (stopped && queue.empty()) {
            return false;
        }

        item = queue.front();

        queue.pop();
        --currentDepth;
        return true;
    }

    bool empty() const {

        std::lock_guard<std::mutex> lock(mutex);

        return queue.empty();
    }

    void stop() {

        {
            std::lock_guard<std::mutex> lock(mutex);

            stopped = true;
        }

        cv.notify_all();
    }
    size_t getCurrentDepth() const {

        return currentDepth.load();
    }

    size_t getPeakDepth() const {

        return peakDepth.load();
    }
};