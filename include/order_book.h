#pragma once

#include "order.h"
#include <queue>
#include <vector>

struct BuyCompare {
    bool operator()(const Order& a, const Order& b) {
        return a.price < b.price;
    }
};

struct SellCompare {
    bool operator()(const Order& a, const Order& b) {
        return a.price > b.price;
    }
};

class OrderBook {
private:
    std::priority_queue<Order, std::vector<Order>, BuyCompare> buyOrders;
    std::priority_queue<Order, std::vector<Order>, SellCompare> sellOrders;

public:
    void addOrder(const Order& order);
    void printBook();
};