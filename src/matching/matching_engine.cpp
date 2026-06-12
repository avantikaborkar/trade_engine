#include "matching/matching_engine.h"
#include "utils/logger.h"
#include <algorithm>
#include <thread>
#include <iostream>
#include <cmath>

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

    orderQueue.stop();
}

void MatchingEngine::matchLoop() {

    Order order;

    while(running) {

        if(!orderQueue.pop(order)) {

            break;
        }
        ordersProcessed++;

        OrderBook& book =
            exchange.getBook(
                order.symbol
            );

        if(order.type == OrderType::STOP) {

            stopOrders.push_back(order);

          /*  Logger::log(
                "[STOP] Registered order "
                + std::to_string(order.orderId)
            );*/

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
            tradesExecuted++;
            recordLatency(order);
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
            tradesExecuted++;
            recordLatency(order);
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
            tradesExecuted++;
            recordLatency(order);
           /* Logger::log(
                "[MARKET] Executed trade for "
                + std::to_string(event.quantity)
                + " shares at price "
                + std::to_string(event.price)
            ); */
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
            tradesExecuted++;
            recordLatency(order);
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
            tradesExecuted++; 
            recordLatency(order);  
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

      /*  Logger::log(
            "[IOC] Cancelled remaining "
            + std::to_string(order.quantity)
            + " shares"
        ); */
    }
}

void MatchingEngine::processFOKOrder(
    Order& order,    OrderBook& book
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

       /* Logger::log(
            "[FOK] Cancelled order "
            + std::to_string(order.orderId)
        );*/

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

          /*  Logger::log(
                "[STOP] Triggered order " +
                std::to_string(it->orderId)
            ); */

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

void MatchingEngine::recordLatency(
    const Order& order
) {

    auto now =
        std::chrono::steady_clock::now();

    auto latency =
        std::chrono::duration_cast<
            std::chrono::microseconds
        >(
            now - order.createdTime
        ).count();

    totalLatencyMicros += latency;

    matchedOrders++;

    uint64_t currentMax =
        maxLatencyMicros.load();

    while(
        latency > currentMax &&
        !maxLatencyMicros.compare_exchange_weak(
            currentMax,
            latency
        )
    ) {
    }
}