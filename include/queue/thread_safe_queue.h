#pragma once

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

public:

    void push(const T& item) {

        {
            std::lock_guard<std::mutex> lock(mutex);

            queue.push(item);
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
};