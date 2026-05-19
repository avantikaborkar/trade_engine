#pragma once

#include "order/order.h"

#include "queue/spsc_queue.h"

#include "risk/risk_engine.h"

#include "journal/journaler.h"

#include <string>

class OrderGateway {

private:
    SPSCQueue<Order>& orderQueue;

    RiskEngine& riskEngine;

    Journaler& journaler;

    int nextOrderId;

public:
    OrderGateway(SPSCQueue<Order>& queue, RiskEngine& risk, Journaler& journal); 

    bool receiveMessage(const std::string& message);

private:
    bool parseMessage(const std::string& message,Order& order); 
};