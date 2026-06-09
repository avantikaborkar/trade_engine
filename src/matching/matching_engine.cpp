#include "matching/matching_engine.h"

#include <algorithm>
#include <thread>
#include <iostream>

MatchingEngine::MatchingEngine(
    Exchange& ex,
    ThreadSafeQueue<Order>& oq,
    SPSCQueue<TradeEvent>& tq
)
    : exchange(ex),
      orderQueue(oq),
      tradeQueue(tq),
      running(true) {}

void MatchingEngine::start() {

    std::thread(
        &MatchingEngine::matchLoop,
        this
    ).detach();
}

void MatchingEngine::stop() {

    running = false;
}

void MatchingEngine::matchLoop() {

    Order order;

    while(running) {

        orderQueue.pop(order);

        OrderBook& book =
            exchange.getBook(
                order.symbol
            );

        if(order.type == OrderType::STOP) {

            stopOrders.push_back(order);

            std::cout
                << "[STOP] Registered order "
                << order.orderId
                << "\n";

            continue;
        }
        if(order.type == OrderType::MARKET) {

            processMarketOrder(
                order,
                book
            );

            continue;
        }
        if(order.type == OrderType::IOC) {

            processIOCOrder(
                order,
                book
            );

            continue;
        }
        if(order.type == OrderType::FOK) {

            processFOKOrder(
                order,
                book
            );

            continue;
        }

        book.addOrder(order);

        while(
            book.getBestBid() != -1 &&
            book.getBestAsk() != -1 &&
            book.getBestBid() >= book.getBestAsk()
        ) {

            int bid = book.getBestBid();

            int ask = book.getBestAsk();

            Order* buy =
                book.getBuyHead(bid);

            Order* sell =
                book.getSellHead(ask);

            int quantity =
                std::min(
                    buy->quantity,
                    sell->quantity
                );

            TradeEvent event(
                order.symbol,
                ask,
                quantity
            );

            tradeQueue.push(event);

            buy->quantity -= quantity;

            sell->quantity -= quantity;

            if(buy->quantity == 0) {

                book.cancelOrder(
                    buy->orderId
                );
            }

            if(sell->quantity == 0) {

                book.cancelOrder(
                    sell->orderId
                );
            }
        }
    }
}

void MatchingEngine::processMarketOrder(
    Order& order,
    OrderBook& book
) {

    while(order.quantity > 0) {

        if(order.side == Side::BUY) {

            int ask = book.getBestAsk();

            if(ask == -1) {

                break;
            }

            Order* sell =
                book.getSellHead(ask);

            int quantity =
                std::min(
                    order.quantity,
                    sell->quantity
                );

            TradeEvent event(
                order.symbol,
                ask,
                quantity
            );

            tradeQueue.push(event);
            checkStopOrders(
                event.price
            );

            order.quantity -= quantity;

            sell->quantity -= quantity;

            if(sell->quantity == 0) {

                book.cancelOrder(
                    sell->orderId
                );
            }
        }
        else {

            int bid = book.getBestBid();

            if(bid == -1) {

                break;
            }

            Order* buy =
                book.getBuyHead(bid);

            int quantity =
                std::min(
                    order.quantity,
                    buy->quantity
                );

            TradeEvent event(
                order.symbol,
                bid,
                quantity
            );

            tradeQueue.push(event);
            checkStopOrders(
                event.price
            );
            order.quantity -= quantity;

            buy->quantity -= quantity;

            if(buy->quantity == 0) {

                book.cancelOrder(
                    buy->orderId
                );
            }
        }
    }
}

void MatchingEngine::processIOCOrder(
    Order& order,
    OrderBook& book
) {

    while(order.quantity > 0) {

        if(order.side == Side::BUY) {

            int ask = book.getBestAsk();

            if(ask == -1) {

                break;
            }

            if(ask > order.price) {

                break;
            }

            Order* sell =
                book.getSellHead(ask);

            int quantity =
                std::min(
                    order.quantity,
                    sell->quantity
                );

            TradeEvent event(
                order.symbol,
                ask,
                quantity
            );

            tradeQueue.push(event);
            checkStopOrders(
                event.price
            );
            order.quantity -= quantity;

            sell->quantity -= quantity;

            if(sell->quantity == 0) {

                book.cancelOrder(
                    sell->orderId
                );
            }
        }
        else {

            int bid = book.getBestBid();

            if(bid == -1) {

                break;
            }

            if(bid < order.price) {

                break;
            }

            Order* buy =
                book.getBuyHead(bid);

            int quantity =
                std::min(
                    order.quantity,
                    buy->quantity
                );

            TradeEvent event(
                order.symbol,
                bid,
                quantity
            );

            tradeQueue.push(event);
            checkStopOrders(        
                event.price
            );
            order.quantity -= quantity;

            buy->quantity -= quantity;

            if(buy->quantity == 0) {

                book.cancelOrder(
                    buy->orderId
                );
            }
        }
    }

    if(order.quantity > 0) {

        std::cout
            << "[IOC] Cancelled remaining "
            << order.quantity
            << " shares\n";
    }
}

void MatchingEngine::processFOKOrder(
    Order& order,
    OrderBook& book
) {

    int available = 0;

    if(order.side == Side::BUY) {

        for(
            int price = book.getBestAsk();
            price != -1 && price <= order.price;
            ++price
        ) {

            Order* cur =
                book.getSellHead(price);

            while(cur) {

                available += cur->quantity;

                cur = cur->next;
            }

            if(available >= order.quantity) {

                break;
            }
        }
    }
    else {

        for(
            int price = book.getBestBid();
            price != -1 && price >= order.price;
            --price
        ) {

            Order* cur =
                book.getBuyHead(price);

            while(cur) {

                available += cur->quantity;

                cur = cur->next;
            }

            if(available >= order.quantity) {

                break;
            }
        }
    }

    if(available < order.quantity) {

        std::cout
            << "[FOK] Cancelled order "
            << order.orderId
            << "\n";

        return;
    }

    processIOCOrder(
        order,
        book
    );
}

void MatchingEngine::checkStopOrders(
    int tradePrice
) {

    auto it = stopOrders.begin();

    while(it != stopOrders.end()) {

        bool triggered = false;

        if(
            it->side == Side::SELL &&
            tradePrice <= it->price
        ) {

            triggered = true;
        }

        if(
            it->side == Side::BUY &&
            tradePrice >= it->price
        ) {

            triggered = true;
        }

        if(triggered) {

            std::cout
                << "[STOP] Triggered order "
                << it->orderId
                << "\n";

            Order marketOrder = *it;

            marketOrder.type =
                OrderType::MARKET;

            it = stopOrders.erase(it);

            OrderBook& book =
                exchange.getBook(
                    marketOrder.symbol
                );

            processMarketOrder(
                marketOrder,
                book
            );
        }
        else {

            ++it;
        }
    }
}