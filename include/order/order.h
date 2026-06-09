#pragma once

#include <string>

enum class Side {
    BUY,
    SELL
};

enum class OrderType {
    LIMIT,
    MARKET,
    IOC,
    FOK,
    STOP
};

struct Order {

    int orderId;

    std::string symbol;

    OrderType type;

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
      type(OrderType::LIMIT),
      side(Side::BUY) {}

    Order(
        int id,
        const std::string& sym,
        int p,
        int q,
        Side s,
        OrderType t = OrderType::LIMIT
    )
        : orderId(id),
        symbol(sym),
        price(p),
        quantity(q),
        side(s),
        type(t) {}
};