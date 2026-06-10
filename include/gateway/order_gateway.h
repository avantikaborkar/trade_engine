#pragma once

#include "order/order.h"
#include "router/symbol_router.h"
#include "queue/thread_safe_queue.h"

#include "risk/risk_engine.h"

#include "journal/journaler.h"

class OrderGateway {

private:

    SymbolRouter& router;

    RiskEngine& riskEngine;

    Journaler& journaler;

    int nextOrderId;

public:

    OrderGateway(
        SymbolRouter& r,
        RiskEngine& risk,
        Journaler& journal
    );

    bool receiveBinaryOrder(
        const std::string& symbol,
        uint8_t side,
        uint8_t orderType,
        int price,
        int quantity
    );

    void setNextOrderId(int id);
};