#pragma once

#include "order.h"
#include <vector>
#include <list>
#include <unordered_map>

class OrderBook {

private:
    static const int MAX_PRICE = 10000;

    std::vector<std::list<Order>> buyLevels;
    std::vector<std::list<Order>> sellLevels;

    std::unordered_map<int, std::list<Order>::iterator> orderMap;

    int bestBid;
    int bestAsk;

public:
    OrderBook();

    void addOrder(const Order& order);
    void cancelOrder(int orderId);
    int getBestBid() const;
    int getBestAsk() const;
    std::list<Order>& getBuyLevel(int price);
    std::list<Order>& getSellLevel(int price);

    void updateBestBid();
    void updateBestAsk();

    void printBook();
};