#include "router/symbol_router.h"
#include <iostream>
#include <functional>

SymbolRouter::SymbolRouter(
    const std::vector<
        std::shared_ptr<
            ThreadSafeQueue<Order>
        >
    >& q
)
    : queues(q) {}

size_t SymbolRouter::getShard(
    const std::string& symbol
) const {

    if(symbol == "AAPL") return 0;
    if(symbol == "MSFT") return 1;
    if(symbol == "GOOG") return 2;
    if(symbol == "AMZN") return 3;

    if(symbol == "META") return 0;
    if(symbol == "TSLA") return 1;
    if(symbol == "NVDA") return 2;

    return 0;
}

void SymbolRouter::routeOrder(
    const Order& order
) {

    auto it = symbolCounts.find(order.symbol);

    if(it != symbolCounts.end())
    {
        it->second++;
    }
    else
    {
        symbolCounts.emplace(order.symbol, 1);
}
    size_t shard =
        getShard(order.symbol);

   /* std::cout
        << "[ROUTER] "
        << order.symbol
        << " -> shard "
        << shard
        << "\n"; */

    queues[shard]->push(order);
}