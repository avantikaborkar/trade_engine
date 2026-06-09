#pragma once

#include "exchange/exchange.h"

#include "queue/thread_safe_queue.h"

#include "queue/spsc_queue.h"

#include "market_data/trade_event.h"

#include <atomic>

#include <vector>

class MatchingEngine {

private:

    Exchange& exchange;

    ThreadSafeQueue<Order>& orderQueue;

    SPSCQueue<TradeEvent>& tradeQueue;

    std::atomic<bool> running;

    std::vector<Order> stopOrders;

    
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

    void processMarketOrder(
        Order& order,
        OrderBook& book
    );
    void processIOCOrder(
        Order& order,
        OrderBook& book
    );
    void processFOKOrder(
        Order& order,
        OrderBook& book
    );

    void checkStopOrders(int tradePrice);
};