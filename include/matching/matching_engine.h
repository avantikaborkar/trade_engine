#pragma once

#include "exchange/exchange.h"

#include "queue/thread_safe_queue.h"

#include "queue/spsc_queue.h"

#include "market_data/trade_event.h"

#include <atomic>

class MatchingEngine {

private:

    Exchange& exchange;

    ThreadSafeQueue<Order>& orderQueue;

    SPSCQueue<TradeEvent>& tradeQueue;

    std::atomic<bool> running;

public:

    MatchingEngine(
        Exchange& ex,
        ThreadSafeQueue<Order>& oq,
        SPSCQueue<TradeEvent>& tq
    );

    void start();

    void stop();

private:

    void matchLoop();
};