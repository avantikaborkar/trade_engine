#include "order/order_book.h"

#include "matching/matching_engine.h"

#include "market_data/market_data_publisher.h"

#include "gateway/order_gateway.h"

#include "risk/risk_engine.h"

#include "journal/journaler.h"

#include "network/tcp_server.h"

#include "queue/spsc_queue.h"

#include <thread>
#include <chrono>
#include <iostream>

int main() {

    OrderBook book;

    SPSCQueue<Order> orderQueue(1024);

    SPSCQueue<TradeEvent> tradeQueue(1024);

    RiskEngine riskEngine(
        1000,
        10000
    );

    Journaler journaler(
        "data/orders.log"
    );

    journaler.replayOrders();

    MatchingEngine engine(
        book,
        orderQueue,
        tradeQueue
    );

    MarketDataPublisher publisher(
        tradeQueue
    );

    OrderGateway gateway(
        orderQueue,
        riskEngine,
        journaler
    );

    TCPServer server(
        8080,
        gateway
    );

    engine.start();

    publisher.start();

    server.start();

    std::cout
        << "\nExchange running...\n";

    std::cout
        << "Use telnet or netcat to send orders\n";

    while(true) {

        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
    }

    return 0;
}