#pragma once

#include "order/order_book.h"

#include <unordered_map>
#include <string>

class Exchange {

private:

    std::unordered_map<
        std::string,
        OrderBook
    > books;

public:

    OrderBook& getBook(
        const std::string& symbol
    );
    const std::unordered_map<
    std::string,
    OrderBook
    >& getAllBooks() const;
};