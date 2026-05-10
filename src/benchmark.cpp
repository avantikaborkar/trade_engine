#include "queue/spsc_queue.h"
#include "matching/matching_engine.h"
#include "order/order_book.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

struct Item {
    int value;
    std::chrono::high_resolution_clock::time_point timestamp;
};

int main() {
    const int N = 1'000'000;

    SPSCQueue<Item> queue(1024);

    std::atomic<bool> done = false;

    long long total_latency = 0;

    //Consumer
    std::thread consumer([&]() {
        Item item;
        int count = 0;

        while (count < N) {
            if (queue.pop(item)) {
                auto now = std::chrono::high_resolution_clock::now();

                auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    now - item.timestamp
                ).count();

                total_latency += latency;
                count++;
            }
        }

        done = true;
    });

    //Producer
    auto start = std::chrono::high_resolution_clock::now();

    std::thread producer([&]() {
        for (int i = 0; i < N; i++) {
            Item item;
            item.value = i;
            item.timestamp = std::chrono::high_resolution_clock::now();

            while (!queue.push(item));
        }
    });

    producer.join();
    consumer.join();

    auto end = std::chrono::high_resolution_clock::now();

    //Throughput
    double seconds = std::chrono::duration<double>(end - start).count();
    double throughput = N / seconds;

    //Latency
    std::cout << "Avg Latency: " << total_latency / N << " ns\n";
    std::cout << "Throughput: " << throughput << " ops/sec\n";

    return 0;
}