#include "order/order_book.h"

#include "exchange/exchange.h"

#include "matching/matching_engine.h"

#include "market_data/market_data_publisher.h"

#include "gateway/order_gateway.h"

#include "risk/risk_engine.h"

#include "journal/journaler.h"

#include "network/tcp_server.h"

#include "queue/spsc_queue.h"
#include "queue/thread_safe_queue.h"
#include <thread>
#include <chrono>
#include <iostream>

int main() {

    Exchange exchange;

    ThreadSafeQueue<Order> orderQueue;

    SPSCQueue<TradeEvent> tradeQueue(1024);

    RiskEngine riskEngine(
        1000,
        10000
    );

    Journaler journaler(
        "data/orders.log"
    );

    journaler.loadSnapshot(
        exchange
    );

    OrderGateway gateway(
        orderQueue,
        riskEngine,
        journaler
    );

    int highestId =
    journaler.getHighestOrderId();

    gateway.setNextOrderId(
        highestId + 1
);

    MatchingEngine engine(
        exchange,
        orderQueue,
        tradeQueue
    );

    MarketDataPublisher publisher(
        tradeQueue
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

    char cmd;

        while(std::cin >> cmd) {

            if(cmd == 'q') {

                journaler.saveSnapshot(exchange);
                if(cmd == 'q') {

                journaler.saveSnapshot(exchange);

                engine.stop();
                publisher.stop();
                server.stop();

            
                }
                break;
            }
        }

return 0;
}