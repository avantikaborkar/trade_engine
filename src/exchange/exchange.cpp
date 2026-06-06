#include "exchange/exchange.h"

OrderBook& Exchange::getBook(
    const std::string& symbol
) {
    return books[symbol];
}

const std::unordered_map<
    std::string,
    OrderBook
>& Exchange::getAllBooks() const {

    return books;
}