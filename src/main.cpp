#include "matching/matching_engine.h"
#include <thread>
#include <chrono>

int main() {

    OrderBook book;
    SPSCQueue<Order> queue(1024);

    MatchingEngine engine(book, queue);
    engine.start();

    std::thread producer([&]() {

        queue.push(Order(1,150,100,Side::BUY));
        queue.push(Order(2,150,50,Side::BUY));

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        queue.push(Order(3,149,80,Side::SELL));
        queue.push(Order(4,149,50,Side::SELL));
    });

    producer.join();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    engine.stop();

    book.printBook();

    return 0;
}