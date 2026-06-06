#include "matching/matching_engine.h"

#include <algorithm>
#include <thread>

MatchingEngine::MatchingEngine(
    Exchange& ex,
    ThreadSafeQueue<Order>& oq,
    SPSCQueue<TradeEvent>& tq
)
    : exchange(ex),
      orderQueue(oq),
      tradeQueue(tq),
      running(true) {}

void MatchingEngine::start() {

    std::thread(
        &MatchingEngine::matchLoop,
        this
    ).detach();
}

void MatchingEngine::stop() {

    running = false;
}

void MatchingEngine::matchLoop() {

    Order order;

    while(running) {

        orderQueue.pop(order);

        OrderBook& book =
            exchange.getBook(
                order.symbol
            );

        book.addOrder(order);

        while(
            book.getBestBid() != -1 &&
            book.getBestAsk() != -1 &&
            book.getBestBid() >= book.getBestAsk()
        ) {

            int bid = book.getBestBid();

            int ask = book.getBestAsk();

            Order* buy =
                book.getBuyHead(bid);

            Order* sell =
                book.getSellHead(ask);

            int quantity =
                std::min(
                    buy->quantity,
                    sell->quantity
                );

            TradeEvent event(
                order.symbol,
                ask,
                quantity
            );

            tradeQueue.push(event);

            buy->quantity -= quantity;

            sell->quantity -= quantity;

            if(buy->quantity == 0) {

                book.cancelOrder(
                    buy->orderId
                );
            }

            if(sell->quantity == 0) {

                book.cancelOrder(
                    sell->orderId
                );
            }
        }
    }
}