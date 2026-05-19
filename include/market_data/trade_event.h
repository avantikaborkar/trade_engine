#pragma once

struct TradeEvent {

    int price;
    int quantity;

    TradeEvent(int p=0, int q=0)
        : price(p), quantity(q) {}
};