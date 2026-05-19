#pragma once

#include "order/order.h"

class RiskEngine {

private:
    int maxOrderQuantity;
    int maxPrice;

public:
    RiskEngine(int maxQty = 1000, int maxP = 10000);
    bool validateOrder(const Order& order);
};