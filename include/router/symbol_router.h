#pragma once

#include "order/order.h"
#include "queue/thread_safe_queue.h"
#include<unordered_map>
#include <vector>
#include <memory>
#include <string>

class SymbolRouter {

private:

    std::vector<
        std::shared_ptr<
            ThreadSafeQueue<Order>
        >
    > queues;

    std::unordered_map<
        std::string,
        uint64_t
    > symbolCounts;

public:

    SymbolRouter(
        const std::vector<
            std::shared_ptr<
                ThreadSafeQueue<Order>
            >
        >& q
    );

    void routeOrder(
        const Order& order
    );

    const std::unordered_map<
        std::string,
        uint64_t
    >& getSymbolCounts() const {

        return symbolCounts;
    }

private:

    size_t getShard(
        const std::string& symbol
    ) const;
};