#pragma once

#include "market_data/trade_event.h"
#include "queue/spsc_queue.h"
#include "order/order_book.h"

#include <atomic>
#include <thread>

class MarketDataPublisher {

private:

    SPSCQueue<TradeEvent>& tradeQueue;

    OrderBook& orderBook;

    std::atomic<bool> running;

public:

    MarketDataPublisher(
        SPSCQueue<TradeEvent>& queue,
        OrderBook& book
    );

    void start();

    void stop();

private:

    void publishLoop();
};