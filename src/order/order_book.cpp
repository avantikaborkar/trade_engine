#include "order_book.h"
#include <iostream>

void OrderBook::addOrder(const Order& order) {

    if(order.side == Side::BUY) {

        auto& orderList = buyBook[order.price];
        orderList.push_back(order);

        auto it = orderList.end();
        --it;

        orderMap[order.orderId] = it;

        if(order.price > bestBid)
            bestBid = order.price;
    }
    else {

        auto& orderList = sellBook[order.price];
        orderList.push_back(order);

        auto it = orderList.end();
        --it;

        orderMap[order.orderId] = it;

        if(bestAsk == -1 || order.price < bestAsk)
            bestAsk = order.price;
    }
}

void OrderBook::cancelOrder(int orderId) {

    auto it = orderMap.find(orderId);

    if(it == orderMap.end()) {
        std::cout << "Order not found\n";
        return;
    }

    auto orderIt = it->second;
    Order order = *orderIt;

    if(order.side == Side::BUY) {

    auto& priceLevel = buyBook[order.price];
    priceLevel.erase(orderIt);

        if(priceLevel.empty())
            buyBook.erase(order.price);

        if(order.price == bestBid && buyBook.size())
            bestBid = buyBook.begin()->first;
    }
    else {

        auto& priceLevel = sellBook[order.price];
        priceLevel.erase(orderIt);

        if(priceLevel.empty())
            sellBook.erase(order.price);

        if(order.price == bestAsk && sellBook.size())
            bestAsk = sellBook.begin()->first;
    }

    orderMap.erase(orderId);
}

void OrderBook::printBook() {

    std::cout << "\n--- BUY BOOK ---\n";

    for(auto& pair : buyBook) {

        int price = pair.first;
        auto& orders = pair.second;

        for(auto& order : orders) {

            std::cout << order.quantity
                      << " @ "
                      << price
                      << "\n";
        }
    }

    std::cout << "\n--- SELL BOOK ---\n";

    for(auto& pair : sellBook) {

        int price = pair.first;
        auto& orders = pair.second;

        for(auto& order : orders) {

            std::cout << order.quantity
                      << " @ "
                      << price
                      << "\n";
        }
    }
}