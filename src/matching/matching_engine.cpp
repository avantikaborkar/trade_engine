#include "matching/matching_engine.h"

#include <algorithm>
#include <thread>

MatchingEngine::MatchingEngine(OrderBook& ob,SPSCQueue<Order>& oq,SPSCQueue<TradeEvent>& tq) : orderBook(ob), orderQueue(oq),  tradeQueue(tq), running(true) {}

void MatchingEngine::start() {

    std::thread(&MatchingEngine::matchLoop, this).detach();
}

void MatchingEngine::stop() {

    running = false;
}

void MatchingEngine::matchLoop() {

    Order order;

    while(running) {

        if(orderQueue.pop(order)) {

            orderBook.addOrder(order);

            while(orderBook.getBestBid() != -1 && orderBook.getBestAsk() != -1 && orderBook.getBestBid() >= orderBook.getBestAsk()) {

                int bid = orderBook.getBestBid();
                int ask = orderBook.getBestAsk();

                Order* buy =orderBook.getBuyHead(bid);

                Order* sell =orderBook.getSellHead(ask);

                int quantity =std::min(buy->quantity, sell->quantity);

                TradeEvent event(ask,quantity);

                tradeQueue.push(event);

                buy->quantity -= quantity;
                sell->quantity -= quantity;

                if(buy->quantity == 0) {

                    orderBook.cancelOrder(buy->orderId);
                }

                if(sell->quantity == 0) {

                    orderBook.cancelOrder(sell->orderId);
                }
            }
        }
    }
}