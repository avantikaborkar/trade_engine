#include "matching/matching_engine.h"
#include <iostream>
#include <thread>
#include <algorithm>

MatchingEngine::MatchingEngine(OrderBook& ob, SPSCQueue<Order>& q): orderBook(ob), queue(q), running(true) {}

void MatchingEngine::start() {
    std::thread(&MatchingEngine::matchLoop, this).detach();
}

void MatchingEngine::stop() {
    running = false;
}

void MatchingEngine::matchLoop() {

    Order order;

    while(running) {

        if(queue.pop(order)) {

            orderBook.addOrder(order);

            while(orderBook.getBestBid() != -1 && orderBook.getBestAsk() != -1 && orderBook.getBestBid() >= orderBook.getBestAsk()) {

                int bid = orderBook.getBestBid();
                int ask = orderBook.getBestAsk();

                Order* buy = orderBook.getBuyHead(bid);
                Order* sell = orderBook.getSellHead(ask);

                int qty = std::min(buy->quantity, sell->quantity);

                std::cout << "TRADE → " << qty << " @ " << ask << "\n";

                buy->quantity -= qty;
                sell->quantity -= qty;

                if(buy->quantity == 0)
                    orderBook.cancelOrder(buy->orderId);

                if(sell->quantity == 0)
                    orderBook.cancelOrder(sell->orderId);
            }
        }
    }
}