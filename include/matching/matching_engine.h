#pragma once

#include "order/order_book.h"

class MatchingEngine {

private:
    OrderBook& orderBook;

public:
    MatchingEngine(OrderBook& ob);

    void processOrder(const Order& order);

private:
    void match();
};