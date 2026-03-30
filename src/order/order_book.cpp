#include "order/order_book.h"
#include <iostream>

OrderBook::OrderBook() : buyLevels(MAX_PRICE + 1),sellLevels(MAX_PRICE + 1),bestBid(-1),bestAsk(-1) {}

void OrderBook::addOrder(const Order& order) {

    if(order.side == Side::BUY) {

        auto& level = buyLevels[order.price];
        level.push_back(order);

        auto it = --level.end();
        orderMap[order.orderId] = it;

        if(order.price > bestBid)
            bestBid = order.price;
    }
    else {

        auto& level = sellLevels[order.price];
        level.push_back(order);

        auto it = --level.end();
        orderMap[order.orderId] = it;

        if(bestAsk == -1 || order.price < bestAsk)
            bestAsk = order.price;
    }
}

void OrderBook::cancelOrder(int orderId) {

    auto it = orderMap.find(orderId);
    if(it == orderMap.end()) return;

    auto orderIt = it->second;
    int price = orderIt->price;
    Side side = orderIt->side;

    if(side == Side::BUY) {

        auto& level = buyLevels[price];
        level.erase(orderIt);

        if(level.empty())
            updateBestBid();
    }
    else {

        auto& level = sellLevels[price];
        level.erase(orderIt);

        if(level.empty())
            updateBestAsk();
    }

    orderMap.erase(orderId);
}

int OrderBook::getBestBid() const { return bestBid; }
int OrderBook::getBestAsk() const { return bestAsk; }

std::list<Order>& OrderBook::getBuyLevel(int price) {
    return buyLevels[price];
}

std::list<Order>& OrderBook::getSellLevel(int price) {
    return sellLevels[price];
}

void OrderBook::updateBestBid() {
    while(bestBid >= 0 && buyLevels[bestBid].empty())
        bestBid--;
    if(bestBid < 0) bestBid = -1;
}

void OrderBook::updateBestAsk() {
    while(bestAsk <= MAX_PRICE && sellLevels[bestAsk].empty())
        bestAsk++;
    if(bestAsk > MAX_PRICE) bestAsk = -1;
}

void OrderBook::printBook() {

    std::cout << "\nBUY\n";
    for(int p = bestBid; p >= 0; --p)
        for(auto& o : buyLevels[p])
            std::cout << o.quantity << " @ " << p << "\n";

    std::cout << "\nSELL\n";
    for(int p = bestAsk; p <= MAX_PRICE; ++p)
        for(auto& o : sellLevels[p])
            std::cout << o.quantity << " @ " << p << "\n";
}