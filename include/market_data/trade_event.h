#pragma once

#include <string>

struct TradeEvent {

    std::string symbol;

    int price;

    int quantity;

    TradeEvent(
        const std::string& sym = "",
        int p = 0,
        int q = 0
    )
        : symbol(sym),
          price(p),
          quantity(q) {}
};