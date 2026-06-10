#include "order/order_book.h"
#include "utils/logger.h"
#include <iostream>
#include <sstream>

OrderBook::OrderBook()
    : buyLevels(MAX_PRICE + 1, nullptr),
      sellLevels(MAX_PRICE + 1, nullptr),
      pool(10000),
      bestBid(-1),
      bestAsk(-1) {}

void OrderBook::addOrder(const Order& order) {

    Order* newOrder = pool.allocate();

    *newOrder = order;
    newOrder->next = nullptr;
    newOrder->prev = nullptr;

    if(order.side == Side::BUY) {

        Order*& head = buyLevels[order.price];

        if(!head) head = newOrder;
        else {
            Order* temp = head;
            while(temp->next) temp = temp->next;
            temp->next = newOrder;
            newOrder->prev = temp;
        }

        if(order.price > bestBid)
            bestBid = order.price;
    }
    else {

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

    if(order->side == Side::BUY) {

        Order*& head = buyLevels[price];

        if(order->prev) order->prev->next = order->next;
        if(order->next) order->next->prev = order->prev;

        if(head == order) head = order->next;

        if(!head) updateBestBid();
    }
    else {

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
    {
        bestBid--;
    }

    if(bestBid < 0) {

        bestBid = -1;
    }
}

void OrderBook::updateBestAsk() {

    while(bestAsk <= MAX_PRICE && !sellLevels[bestAsk])
    {
        bestAsk++;
    }

    if(bestAsk > MAX_PRICE) {

        bestAsk = -1;
    }
}


void OrderBook::printBook() {

    std::ostringstream out;

    out << "\nSELL:\n";

    if(bestAsk != -1) {

        for(int p = bestAsk; p <= MAX_PRICE; ++p) {

            Order* cur = sellLevels[p];

            while(cur) {

                out
                    << cur->quantity
                    << " x "
                    << p
                    << "\n";

                cur = cur->next;
            }
        }
    }

    out << "\nBUY:\n";

    if(bestBid != -1) {

        for(int p = bestBid; p >= 0; --p) {

            Order* cur = buyLevels[p];

            while(cur) {

                out
                    << cur->quantity
                    << " x "
                    << p
                    << "\n";

                cur = cur->next;
            }
        }
    }

    Logger::log(out.str());
}

std::vector<Order> OrderBook::getAllOrders() const {

    std::vector<Order> orders;

    for(int price = 0; price <= MAX_PRICE; ++price) {

        Order* cur = buyLevels[price];

        while(cur) {

            orders.push_back(*cur);

            cur = cur->next;
        }
    }

    for(int price = 0; price <= MAX_PRICE; ++price) {

        Order* cur = sellLevels[price];

        while(cur) {

            orders.push_back(*cur);

            cur = cur->next;
        }
    }

    return orders;
}

void OrderBook::clear() {

    orderMap.clear();

    for(auto& level : buyLevels) {

        level = nullptr;
    }

    for(auto& level : sellLevels) {

        level = nullptr;
    }

    bestBid = -1;

    bestAsk = -1;
}