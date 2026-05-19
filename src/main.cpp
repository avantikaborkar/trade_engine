#include "order/order_book.h"

#include "matching/matching_engine.h"

#include "market_data/market_data_publisher.h"

#include "gateway/order_gateway.h"

#include "risk/risk_engine.h"

#include "journal/journaler.h"

#include "queue/spsc_queue.h"

#include <thread>
#include <chrono>

int main() {

    OrderBook book;

    SPSCQueue<Order> orderQueue(1024);

    SPSCQueue<TradeEvent> tradeQueue(1024);

    RiskEngine riskEngine(1000,10000);

    Journaler journaler("data/orders.log");

    journaler.replayOrders();

    MatchingEngine engine(book,orderQueue,tradeQueue);

    MarketDataPublisher publisher(tradeQueue);

    OrderGateway gateway(orderQueue,riskEngine,journaler);

    engine.start();

    publisher.start();

    gateway.receiveMessage("BUY 150 100");

    gateway.receiveMessage("SELL 149 80");

    gateway.receiveMessage("BUY 155 50");

    std::this_thread::sleep_for(std::chrono::seconds(1));

    engine.stop();

    publisher.stop();

    book.printBook();

    return 0;
}