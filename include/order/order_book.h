#pragma once

#include "order.h"
#include <vector>
#include "memory/memory_pool.h"
#include <unordered_map>

class OrderBook {

private:
    static const int MAX_PRICE = 10000;

    std::vector<Order*> buyLevels;
    std::vector<Order*> sellLevels;
    std::unordered_map<int, Order*> orderMap;
    MemoryPool<Order> pool;
    int bestBid;
    int bestAsk;

public:
    OrderBook();
    void addOrder(const Order& order);
    void cancelOrder(int orderId);
    int getBestBid() const;
    int getBestAsk() const;
    Order* getBuyHead(int price);
    Order* getSellHead(int price);
    void updateBestBid();
    void updateBestAsk();

    void printBook();
};