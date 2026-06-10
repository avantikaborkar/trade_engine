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

    return std::hash<std::string>{}(symbol)
        % queues.size();
}

void SymbolRouter::routeOrder(
    const Order& order
) {

    symbolCounts[order.symbol]++;
    size_t shard =
        getShard(order.symbol);

    std::cout
        << "[ROUTER] "
        << order.symbol
        << " -> shard "
        << shard
        << "\n";

    queues[shard]->push(order);
}