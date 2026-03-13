#pragma once
using namespace std;
#include "order.h"
#include <map>
#include <list>
#include <unordered_map>

class OrderBook {

private:

    // BUY side (highest price first)
    map<int, std::list<Order>, std::greater<int>> buyBook;

    // SELL side (lowest price first)
    map<int, std::list<Order>> sellBook;

    // orderId → iterator to order
    unordered_map<int, std::list<Order>::iterator> orderMap;
    int bestBid=-1;
    int bestAsk=-1;

public:

    void addOrder(const Order& order);
    void cancelOrder(int orderId);
    void printBook();
    int getBestBid() const { return bestBid; }
    int getBestAsk() const { return bestAsk; }
};