#pragma once
#include "exchange/exchange.h"
#include "order/order.h"
#include "order/order_book.h"

#include <string>

class Journaler {

private:

    std::string filename;

public:

    Journaler(
        const std::string& file
    );

    void logOrder(
        const Order& order
    );

    void replayOrders(
        OrderBook& book
    );

    void saveSnapshot(
        Exchange& exchange
    );

    void loadSnapshot(
        Exchange& exchange
    );
    int getHighestOrderId();
};