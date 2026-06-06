#pragma once

#include "market_data/trade_event.h"
#include "queue/spsc_queue.h"

#include <atomic>
#include <thread>

class MarketDataPublisher {

private:

    SPSCQueue<TradeEvent>& tradeQueue;

    std::atomic<bool> running;

public:

    MarketDataPublisher(
        SPSCQueue<TradeEvent>& queue
    );

    void start();

    void stop();

private:

    void publishLoop();
};