#include "order/order_book.h"
#include <iostream>

OrderBook::OrderBook(): buyLevels(MAX_PRICE + 1, nullptr),  sellLevels(MAX_PRICE + 1, nullptr),  pool(10000),  bestBid(-1),  bestAsk(-1) {}

void OrderBook::addOrder(const Order& order) {

    Order* newOrder = pool.allocate();
    *newOrder = order;
    newOrder->next = nullptr;
    newOrder->prev = nullptr;

    if(order.side == Side::BUY) 
    {

        Order*& head = buyLevels[order.price];

        if(!head) head = newOrder;
        else 
        {
            Order* temp = head;
            while(temp->next) temp = temp->next;
            temp->next = newOrder;
            newOrder->prev = temp;
        }

        if(order.price > bestBid)
            bestBid = order.price;
    }
    else 
    {

        Order*& head = sellLevels[order.price];

        if(!head) head = newOrder;
        else {
            Order* temp = head;
            while(temp->next) temp = temp->next;
            temp->next = newOrder;
            newOrder->prev = temp;
        }

        if(bestAsk == -1 || order.price < bestAsk)
            bestAsk = order.price;
    }

    orderMap[order.orderId] = newOrder;
}

void OrderBook::cancelOrder(int orderId) {

    auto it = orderMap.find(orderId);
    if(it == orderMap.end()) return;

    Order* order = it->second;
    int price = order->price;

    if(order->side == Side::BUY) 
    {

        Order*& head = buyLevels[price];

        if(order->prev) order->prev->next = order->next;
        if(order->next) order->next->prev = order->prev;

        if(head == order) head = order->next;

        if(!head) updateBestBid();
    }
    else 
    {

        Order*& head = sellLevels[price];

        if(order->prev) order->prev->next = order->next;
        if(order->next) order->next->prev = order->prev;

        if(head == order) head = order->next;

        if(!head) updateBestAsk();
    }

    pool.deallocate(order);
    orderMap.erase(orderId);
}

int OrderBook::getBestBid() const { return bestBid; }
int OrderBook::getBestAsk() const { return bestAsk; }

Order* OrderBook::getBuyHead(int price) { return buyLevels[price]; }
Order* OrderBook::getSellHead(int price) { return sellLevels[price]; }

void OrderBook::updateBestBid() {
    while(bestBid >= 0 && !buyLevels[bestBid])
        bestBid--;
}

void OrderBook::updateBestAsk() {
    while(bestAsk <= MAX_PRICE && !sellLevels[bestAsk])
    {
        bestAsk++;
        if(bestAsk > MAX_PRICE) {
            bestAsk = -1;
            break;
        }
    }
}

void OrderBook::printBook() {

    std::cout << "\nBUY\n";
    for(int p = bestBid; p >= 0; --p) {
        Order* cur = buyLevels[p];
        while(cur) {
            std::cout << cur->quantity << " @ " << p << "\n";
            cur = cur->next;
        }
    }

    std::cout << "\nSELL\n";
    for(int p = bestAsk; p <= MAX_PRICE; ++p) {
        Order* cur = sellLevels[p];
        while(cur) {
            std::cout << cur->quantity << " @ " << p << "\n";
            cur = cur->next;
        }
    }
}