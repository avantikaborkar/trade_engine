#pragma once

#include <string>

enum class Side {
    BUY,
    SELL
};

struct Order {

    int orderId;

    std::string symbol;

    int price;

    int quantity;

    Side side;

    Order* next = nullptr;

    Order* prev = nullptr;

    Order()
        : orderId(0),
          symbol("AAPL"),
          price(0),
          quantity(0),
          side(Side::BUY) {}

    Order(
        int id,
        const std::string& sym,
        int p,
        int q,
        Side s
    )
        : orderId(id),
          symbol(sym),
          price(p),
          quantity(q),
          side(s) {}
};