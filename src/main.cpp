#include "matching/matching_engine.h"
#include "order/order_book.h"

int main() {

    OrderBook book;
    MatchingEngine engine(book);

    engine.processOrder(Order(1, 150, 100, Side::BUY));
    engine.processOrder(Order(2, 150, 50, Side::BUY));

    engine.processOrder(Order(3, 200, 80, Side::SELL));
    engine.processOrder(Order(4, 200, 50, Side::SELL));

    book.printBook();

    return 0;
}