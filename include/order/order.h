#pragma once

#include <string>

enum class Side {
    BUY,
    SELL
};

struct Order {
    int orderId;
    int price;
    int quantity;
    Side side;

    Order(int id, int p, int q, Side s)
        : orderId(id), price(p), quantity(q), side(s) {}
};