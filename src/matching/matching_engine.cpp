#include "matching/matching_engine.h"
#include "order/order_book.h"
#include <iostream>
#include <algorithm>

MatchingEngine::MatchingEngine(OrderBook& ob) : orderBook(ob) {}

void MatchingEngine::processOrder(const Order& order) {

    orderBook.addOrder(order);

    match();
}

void MatchingEngine::match() {

    while(orderBook.getBestBid() != -1 && orderBook.getBestAsk() != -1 && orderBook.getBestBid() >= orderBook.getBestAsk()) {

        int bid = orderBook.getBestBid();
        int ask = orderBook.getBestAsk();

        auto& buyQ = orderBook.getBuyLevel(bid);
        auto& sellQ = orderBook.getSellLevel(ask);

        auto& buy = buyQ.front();
        auto& sell = sellQ.front();

        int qty = std::min(buy.quantity, sell.quantity);
        int price = ask;

        std::cout << "trade: " << qty << " @ " << price << "\n";

        buy.quantity -= qty;
        sell.quantity -= qty;

        if(buy.quantity == 0)
            orderBook.cancelOrder(buy.orderId);

        if(sell.quantity == 0)
            orderBook.cancelOrder(sell.orderId);
    }
}