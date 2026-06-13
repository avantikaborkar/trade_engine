#pragma once

#include "exchange/exchange.h"
#include "order/order.h"
#include "order/order_book.h"
#include "journal/journal_entry.h"
#include "queue/thread_safe_queue.h"

#include <string>
#include <thread>
#include <atomic>

class Journaler {

private:

    std::string filename;

    ThreadSafeQueue<JournalEntry> queue;

    std::thread writerThread;

    std::atomic<bool> running;

public:

    Journaler(
        const std::string& file
    );

    ~Journaler();

    void logOrder(
        const Order& order
    );

    void replayOrders(
        OrderBook& book
    );

    void saveSnapshot(
        Exchange& exchange
    );

    void loadSnapshot(
        Exchange& exchange
    );

    int getHighestOrderId();

private:

    void writerLoop();
};