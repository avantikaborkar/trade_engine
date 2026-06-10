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

    std::atomic<uint64_t> ordersProcessed{0};

    std::atomic<uint64_t> tradesExecuted{0};

    std::atomic<uint64_t> totalLatencyMicros{0};

    std::atomic<uint64_t> matchedOrders{0};

    std::atomic<uint64_t> maxLatencyMicros{0};

    
public:

    MatchingEngine(
        Exchange& ex,
        ThreadSafeQueue<Order>& oq,
        SPSCQueue<TradeEvent>& tq
    );

    uint64_t getOrdersProcessed() const {

        return ordersProcessed.load();
    }

    uint64_t getTradesExecuted() const {

        return tradesExecuted.load();
    }

    void start();

    void stop();

    double getAverageLatencyMicros() const {

        uint64_t count =
            matchedOrders.load();

        if(count == 0) {

            return 0.0;
        }

        return static_cast<double> (totalLatencyMicros.load()) / count;
    }

    uint64_t getMaxLatencyMicros() const {

        return maxLatencyMicros.load();
    }

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

    void recordLatency(
        const Order& order
    );
    void checkStopOrders(int tradePrice);
};