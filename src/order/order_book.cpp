#include "order/order_book.h"
#include "utils/logger.h"
#include <iostream>
#include <sstream>

OrderBook::OrderBook()
    : buyLevels(MAX_PRICE + 1),
      sellLevels(MAX_PRICE + 1),
      pool(2000000),
      bestBid(-1),
      bestAsk(-1) {}

void OrderBook::addOrder(const Order& order) {

    Order* newOrder = pool.allocate();

        if(newOrder == nullptr)
        {
           /* Logger::log(
                "[ORDERBOOK] Memory pool exhausted"
            );*/
            return;
        }

    *newOrder = order;
    newOrder->next = nullptr;
    newOrder->prev = nullptr;

    if(order.side == Side::BUY) {

        PriceLevel& level = buyLevels[order.price];

    if(!level.head)
    {
        level.head = level.tail = newOrder;
    }
    else
    {
        level.tail->next = newOrder;
        newOrder->prev = level.tail;
        level.tail = newOrder;
    }

        if(order.price > bestBid)
            bestBid = order.price;
    }
    else {

        PriceLevel& level = sellLevels[order.price];

        if(!level.head)
        {
            level.head = level.tail = newOrder;
        }
        else
        {
            level.tail->next = newOrder;
            newOrder->prev = level.tail;
            level.tail = newOrder;
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

        PriceLevel& level = buyLevels[price];

        if(order->prev)
            order->prev->next = order->next;

        if(order->next)
            order->next->prev = order->prev;

        if(level.head == order)
            level.head = order->next;

        if(level.tail == order)
            level.tail = order->prev;

        if(!level.head)
            updateBestBid();
    }
    else {

       PriceLevel& level = sellLevels[price];

        if(order->prev)
            order->prev->next = order->next;

        if(order->next)
            order->next->prev = order->prev;

        if(level.head == order)
            level.head = order->next;

        if(level.tail == order)
            level.tail = order->prev;

        if(!level.head)
            updateBestAsk();
    }

    pool.deallocate(order);
    orderMap.erase(orderId);
}

int OrderBook::getBestBid() const { return bestBid; }
int OrderBook::getBestAsk() const { return bestAsk; }

Order* OrderBook::getBuyHead(int price)
{
    return buyLevels[price].head;
}
Order* OrderBook::getSellHead(int price)
{
    return sellLevels[price].head;
}

void OrderBook::updateBestBid() {

    while(bestBid >= 0 && buyLevels[bestBid].head == nullptr)
    {
        bestBid--;
    }

    if(bestBid < 0) {

        bestBid = -1;
    }
}

void OrderBook::updateBestAsk() {

    while(bestAsk <= MAX_PRICE && sellLevels[bestAsk].head == nullptr)
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

            Order* cur = sellLevels[p].head;

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

            Order* cur = buyLevels[p].head;

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

        Order* cur = buyLevels[price].head;

        while(cur) {

            orders.push_back(*cur);

            cur = cur->next;
        }
    }

    for(int price = 0; price <= MAX_PRICE; ++price) {

        Order* cur = sellLevels[price].head;

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

        level.head = nullptr;
        level.tail = nullptr;       
    }

    for(auto& level : sellLevels) {

        level.head = nullptr;
        level.tail = nullptr;       
    }

    bestBid = -1;

    bestAsk = -1;
}