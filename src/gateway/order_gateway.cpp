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
    uint8_t orderTypeValue,
    int price,
    int quantity
) {

    Side side;

    OrderType type;

    if(orderTypeValue == 1) {

        type = OrderType::LIMIT;
    }
    else if(orderTypeValue == 2) {

        type = OrderType::MARKET;
    }
    else if(orderTypeValue == 3) {

        type = OrderType::IOC;
    }
    else if(orderTypeValue == 4) {

        type = OrderType::FOK;
    }
    else if(orderTypeValue == 5) {

        type = OrderType::STOP;
    }
    else {

        std::cout
            << "[GATEWAY] Invalid order type\n";

        return false;
    }

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
        side,
        type
    );

    if(!riskEngine.validateOrder(order)) {

        std::cout
        << "[GATEWAY] Accepted "
        << (type == OrderType::MARKET ? "MARKET ": "LIMIT ")
        << symbol
        << " order "
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