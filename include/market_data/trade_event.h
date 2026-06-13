#pragma once

#include <string>

struct TradeEvent {

    std::string symbol;

    int price;

    int quantity;

    int bestBid;

    int bestAsk;

    TradeEvent(
        const std::string& sym = "",
        int p = 0,
        int q = 0,
        int bid = -1,
        int ask = -1
    )
        : symbol(sym),
          price(p),
          quantity(q),
          bestBid(bid),
          bestAsk(ask) {}
};