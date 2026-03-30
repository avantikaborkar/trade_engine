#pragma once

#include "order/order_book.h"
#include "queue/spsc_queue.h"
#include <atomic>

class MatchingEngine {

private:
    OrderBook& orderBook;
    SPSCQueue<Order>& queue;
    std::atomic<bool> running;

public:
    MatchingEngine(OrderBook& ob, SPSCQueue<Order>& q);

    void start();
    void stop();

private:
    void matchLoop();
};