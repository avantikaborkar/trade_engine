#include "order/order_book.h"

#include "exchange/exchange.h"

#include "matching/matching_engine.h"

#include "market_data/market_data_publisher.h"

#include "gateway/order_gateway.h"

#include "risk/risk_engine.h"

#include "journal/journaler.h"

#include "network/tcp_server.h"

#include "router/symbol_router.h"

#include <memory>
#include <vector>

#include "queue/spsc_queue.h"

#include <thread>
#include <chrono>
#include <iostream>

int main() {

    Exchange exchange;

    SPSCQueue<Order> orderQueue0(100000);
    SPSCQueue<Order> orderQueue1(100000);
    SPSCQueue<Order> orderQueue2(100000);
    SPSCQueue<Order> orderQueue3(100000);

    std::vector<
    std::shared_ptr<
        SPSCQueue<Order>
    >
        > queues {

            std::shared_ptr<
                SPSCQueue<Order>
            >(&orderQueue0, [](auto*) {}),

            std::shared_ptr<
                SPSCQueue<Order>
            >(&orderQueue1, [](auto*) {}),

            std::shared_ptr<
                SPSCQueue<Order>
            >(&orderQueue2, [](auto*) {}),

            std::shared_ptr<
                SPSCQueue<Order>
            >(&orderQueue3, [](auto*) {})
        };

        SymbolRouter router(
            queues
        );
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
        router,
        riskEngine,
        journaler
    );

    int highestId =
    journaler.getHighestOrderId();

    gateway.setNextOrderId(
        highestId + 1
);

        MatchingEngine engine0(
        exchange,
        orderQueue0,
        tradeQueue
    );

    MatchingEngine engine1(
        exchange,
        orderQueue1,
        tradeQueue
    );

    MatchingEngine engine2(
        exchange,
        orderQueue2,
        tradeQueue
    );

    MatchingEngine engine3(
        exchange,
        orderQueue3,
        tradeQueue
    );
    MarketDataPublisher publisher(
        tradeQueue
    );


    TCPServer server(
        8080,
        gateway
    );

    engine0.start();
    engine1.start();
    engine2.start();
    engine3.start();

    publisher.start();

    server.start();

    std::cout
        << "\nExchange running...\n";

    std::cout
        << "Use telnet or netcat to send orders\n";

    char cmd;

        while(std::cin >> cmd) {    
            
            if(cmd == 'h' || cmd == 'H') {

                std::cout
                    << "\n=== AVAILABLE COMMANDS ===\n";

                std::cout
                    << "h           - Help\n";

                std::cout
                    << "m           - Engine Metrics\n";

                std::cout
                    << "l           - Latency Metrics\n";

                std::cout
                    << "r           - Router Metrics\n";

                std::cout
                    << "b <symbol>  - Print Order Book\n";

                std::cout
                    << "q           - Save Snapshot & Quit\n";

                continue;
            }

            if(cmd == 'm' || cmd == 'M') {

                std::cout
                    << "\n=== ENGINE METRICS ===\n";

                std::cout
                    << "Engine0 Orders: "
                    << engine0.getOrdersProcessed()
                    << " Trades: "
                    << engine0.getTradesExecuted()
                    << "\n";

                std::cout
                    << "Engine1 Orders: "
                    << engine1.getOrdersProcessed()
                    << " Trades: "
                    << engine1.getTradesExecuted()
                    << "\n";

                std::cout
                    << "Engine2 Orders: "
                    << engine2.getOrdersProcessed()
                    << " Trades: "
                    << engine2.getTradesExecuted()
                    << "\n";

                std::cout
                    << "Engine3 Orders: "
                    << engine3.getOrdersProcessed()
                    << " Trades: "
                    << engine3.getTradesExecuted()
                    << "\n";
                    std::cout
                    << "\n=== QUEUE METRICS ===\n";

                std::cout
                    << "Q0 Current: "
                    << orderQueue0.getCurrentDepth()
                    << " Peak: "
                    << orderQueue0.getPeakDepth()
                    << "\n";

                std::cout
                    << "Q1 Current: "
                    << orderQueue1.getCurrentDepth()
                    << " Peak: "
                    << orderQueue1.getPeakDepth()
                    << "\n";

                std::cout
                    << "Q2 Current: "
                    << orderQueue2.getCurrentDepth()
                    << " Peak: "
                    << orderQueue2.getPeakDepth()
                    << "\n";

                std::cout
                    << "Q3 Current: "
                    << orderQueue3.getCurrentDepth()
                    << " Peak: "
                    << orderQueue3.getPeakDepth()
                    << "\n";

                uint64_t totalOrders =
                    engine0.getOrdersProcessed()
                    + engine1.getOrdersProcessed()
                    + engine2.getOrdersProcessed()
                    + engine3.getOrdersProcessed();

                std::cout
                    << "\n=== ENGINE UTILIZATION ===\n";

                std::cout
                    << "Engine0: "
                    << (100.0 *
                        engine0.getOrdersProcessed()
                        / totalOrders)
                    << "%\n";

                std::cout
                    << "Engine1: "
                    << (100.0 *
                        engine1.getOrdersProcessed()
                        / totalOrders)
                    << "%\n";

                std::cout
                    << "Engine2: "
                    << (100.0 *
                        engine2.getOrdersProcessed()
                        / totalOrders)
                    << "%\n";

                std::cout
                    << "Engine3: "
                    << (100.0 *
                        engine3.getOrdersProcessed()
                        / totalOrders)
                    << "%\n";
                continue;
            }
            if(cmd == 'r' || cmd == 'R') {

                std::cout
                    << "\n=== ROUTER METRICS ===\n";

                const auto& counts =
                    router.getSymbolCounts();

                for(const auto& entry : counts) {

                    std::cout
                        << entry.first
                        << " : "
                        << entry.second
                        << "\n";
                }

                continue;
            }

            if(cmd == 'l' || cmd == 'L') {

                std::cout
                    << "\n=== LATENCY METRICS ===\n";

                std::cout
                    << "Engine0 Avg: "
                    << engine0.getAverageLatencyMicros()
                    << " us  Max: "
                    << engine0.getMaxLatencyMicros()
                    << " us\n";

                std::cout
                    << "Engine1 Avg: "
                    << engine1.getAverageLatencyMicros()
                    << " us  Max: "
                    << engine1.getMaxLatencyMicros()
                    << " us\n";

                std::cout
                    << "Engine2 Avg: "
                    << engine2.getAverageLatencyMicros()
                    << " us  Max: "
                    << engine2.getMaxLatencyMicros()
                    << " us\n";

                std::cout
                    << "Engine3 Avg: "
                    << engine3.getAverageLatencyMicros()
                    << " us  Max: "
                    << engine3.getMaxLatencyMicros()
                    << " us\n";

                continue;
            }

            if(cmd == 'b' || cmd == 'B') {

                std::string symbol;

                std::cin >> symbol;

                auto& book =
                    exchange.getBook(symbol);

                std::cout
                    << "\n=== "
                    << symbol
                    << " ORDER BOOK ===\n";

                book.printBook();

                continue;
            }

            if(cmd == 'q' || cmd == 'Q') {

                journaler.saveSnapshot(exchange);

                engine0.stop();
                engine1.stop();
                engine2.stop();
                engine3.stop();

                publisher.stop();

                server.stop();

                break;
            }
        }

return 0;
}