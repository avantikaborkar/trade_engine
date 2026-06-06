#include "journal/journaler.h"

#include <fstream>
#include <iostream>
#include <sstream>

Journaler::Journaler(
    const std::string& file
)
    : filename(file) {}

void Journaler::logOrder(
    const Order& order
) {

    std::ofstream out(
        filename,
        std::ios::app
    );

    if(!out.is_open()) {

        std::cout
            << "NOTE: Failed to open log file\n";

        return;
    }

    out
        << order.orderId << " "
        << (order.side == Side::BUY ? "BUY" : "SELL")
        << " "
        << order.price << " "
        << order.quantity
        << "\n";
}

void Journaler::replayOrders(
    OrderBook& book
) {

    std::ifstream in(filename);

    if(!in.is_open()) {

        std::cout<< "NOTE: No previous log found\n";

        return;
    }

    std::cout<< "\nReplaying Orders:\n";

    std::string line;

    while(std::getline(in, line)) {

        std::cout<< line<< "\n";

        std::stringstream ss(line);

        int id;
        std::string sideStr;
        int price;
        int qty;

        ss>> id>> sideStr>> price>> qty;

        Side side =(sideStr == "BUY")? Side::BUY: Side::SELL;

        Order order(id,"AAPL", price, qty, side);

        book.addOrder(order);
    }
}

void Journaler::saveSnapshot(
    Exchange& exchange
) {

    std::ofstream out(
        "data/snapshot.dat"
    );

    if(!out.is_open()) {

        return;
    }

    const auto& books =
        exchange.getAllBooks();

    for(const auto& pair : books) {

        const std::string& symbol =
            pair.first;

        const OrderBook& book =
            pair.second;

        auto orders =
            book.getAllOrders();

        for(const auto& order : orders) {

            out
                << order.orderId << " "
                << symbol << " "
                << (order.side == Side::BUY
                    ? "BUY"
                    : "SELL")
                << " "
                << order.price << " "
                << order.quantity
                << "\n";
        }
    }

    std::cout
        << "[SNAPSHOT] Saved\n";
}

void Journaler::loadSnapshot(
    Exchange& exchange
) {

    std::ifstream in(
        "data/snapshot.dat"
    );

    if(!in.is_open()) {

        std::cout
            << "[SNAPSHOT] No snapshot found\n";

        return;
    }

    int id;
    std::string symbol;
    std::string sideStr;
    int price;
    int qty;

    while(
        in
        >> id
        >> symbol
        >> sideStr
        >> price
        >> qty
    ) {

        Side side =
            (sideStr == "BUY")
            ? Side::BUY
            : Side::SELL;

        Order order(
            id,
            symbol,
            price,
            qty,
            side
        );

        exchange
            .getBook(symbol)
            .addOrder(order);
    }

    std::cout
        << "[SNAPSHOT] Loaded\n";
}

int Journaler::getHighestOrderId() {

    std::ifstream in("data/snapshot.dat");

    if(!in.is_open()) {
        return 0;
    }

    int maxId = 0;

    int id;
    std::string symbol;
    std::string sideStr;
    int price;
    int qty;

    while(
        in
        >> id
        >> symbol
        >> sideStr
        >> price
        >> qty
    ) {

        if(id > maxId) {
            maxId = id;
        }
    }

    return maxId;
}