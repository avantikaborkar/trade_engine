#pragma once

#include "order/order.h"

#include <string>

class Journaler {

private:
    std::string filename;

public:
    Journaler(const std::string& file);

    void logOrder(const Order& order);

    void replayOrders();
};