#include "gateway/order_gateway.h"

#include <iostream>

OrderGateway::OrderGateway(
    ThreadSafeQueue<Order>& queue,
    RiskEngine& risk,
    Journaler& journal
)
    : orderQueue(queue),
      riskEngine(risk),
      journaler(journal),
      nextOrderId(1) {}

bool OrderGateway::receiveBinaryOrder(
    const std::string& symbol,
    uint8_t sideValue,
    int price,
    int quantity
) {

    Side side;

    if(sideValue == 1) {

        side = Side::BUY;
    }
    else if(sideValue == 2) {

        side = Side::SELL;
    }
    else {

        std::cout
            << "[GATEWAY] Invalid side\n";

        return false;
    }

    Order order(
        nextOrderId++,
        symbol,
        price,
        quantity,
        side
    );

    if(!riskEngine.validateOrder(order)) {

        std::cout
            << "[GATEWAY] Risk rejected order "
            << order.orderId
            << "\n";

        return false;
    }

    journaler.logOrder(order);

    orderQueue.push(order);

    std::cout
        << "[GATEWAY] Accepted "
        << symbol
        << " order "
        << order.orderId
        << "\n";

    return true;
}

void OrderGateway::setNextOrderId(int id) {
    nextOrderId = id;
}