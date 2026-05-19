#include "gateway/order_gateway.h"

#include <sstream>
#include <iostream>

OrderGateway::OrderGateway(SPSCQueue<Order>& queue,RiskEngine& risk,Journaler& journal)
    : orderQueue(queue), riskEngine(risk), journaler(journal), nextOrderId(1) {}

bool OrderGateway::receiveMessage(const std::string& message) {

    Order order;

    if(!parseMessage(message, order)) {

        std::cout << "[GATEWAY] Invalid message"<<std::endl;
        return false;
    }

    if(!riskEngine.validateOrder(order)) {

        std::cout<< "[GATEWAY] Risk rejected order "<< order.orderId<< std::endl;

        return false;
    }

    journaler.logOrder(order);

    if(!orderQueue.push(order)) {

        std::cout << "[GATEWAY] Queue full "<< std::endl;

        return false;
    }

    std::cout<< "[GATEWAY] Accepted order "<< order.orderId<< std::endl;

    return true;
}

bool OrderGateway::parseMessage(const std::string& message,Order& order) {

    std::stringstream ss(message);

    std::string sideStr;

    int price;
    int quantity;

    ss >> sideStr >> price >> quantity;

    if(ss.fail()) {
        return false;
    }

    Side side;

    if(sideStr == "BUY") {

        side = Side::BUY;
    }
    else if(sideStr == "SELL") {

        side = Side::SELL;
    }
    else {

        return false;
    }

    order = Order(nextOrderId++,price,quantity,side);

    return true;
}