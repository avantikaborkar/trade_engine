#include "order_book.h"

int main() {

    OrderBook book;

    book.addOrder(Order(1,150,100,Side::BUY));
    book.addOrder(Order(2,150,50,Side::BUY));
    book.addOrder(Order(3,151,200,Side::SELL));
    book.addOrder(Order(4,152,100,Side::SELL));

    book.printBook();

    book.cancelOrder(2);

    book.printBook();

}