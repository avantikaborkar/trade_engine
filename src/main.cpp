#include "order/order_book.h"

int main() {

    OrderBook book;

    book.addOrder(Order(101,150,100,Side::BUY));
    book.addOrder(Order(102,150,50,Side::BUY));
    book.addOrder(Order(103,151,200,Side::SELL));
    book.addOrder(Order(104,152,100,Side::SELL));
    book.printBook();
    book.cancelOrder(102);
    book.printBook();

}