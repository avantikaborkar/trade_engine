#pragma once

#include "order/order.h"

#include "queue/thread_safe_queue.h"

#include "risk/risk_engine.h"

#include "journal/journaler.h"

class OrderGateway {

private:

    ThreadSafeQueue<Order>& orderQueue;

    RiskEngine& riskEngine;

    Journaler& journaler;

    int nextOrderId;

public:

    OrderGateway(
        ThreadSafeQueue<Order>& queue,
        RiskEngine& risk,
        Journaler& journal
    );

    bool receiveBinaryOrder(
    const std::string& symbol,
    uint8_t side,
    int price,
    int quantity
    );

    void setNextOrderId(int id);
};