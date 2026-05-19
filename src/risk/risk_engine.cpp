#include "risk/risk_engine.h"

#include <iostream>

RiskEngine::RiskEngine(int maxQty, int maxP) : maxOrderQuantity(maxQty), maxPrice(maxP) {}

bool RiskEngine::validateOrder(const Order& order) 
{

    if(order.quantity <= 0) {

        std::cout << "RISK-> Invalid quantity\n";
        return false;
    }

    if(order.quantity > maxOrderQuantity) {

        std::cout << "RISK-> Quantity limit exceeded\n";
        return false;
    }

    if(order.price <= 0) {

        std::cout<< "RISK-> Invalid price\n";
        return false;
    }

    if(order.price > maxPrice) {

        std::cout<< "RISK-> Price limit exceeded\n";
        return false;
    }

    return true;
}