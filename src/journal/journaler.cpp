#include "journal/journaler.h"

#include <fstream>
#include <iostream>
#include <sstream>

Journaler::Journaler(const std::string& file) : filename(file) {}

void Journaler::logOrder(const Order& order) {

    std::ofstream out(filename, std::ios::app);

    if(!out.is_open()) {

        std::cout<< "NOTE: Failed to open log file\n";

        return;
    }

    out<< order.orderId << " "<< (order.side == Side::BUY ? "BUY" : "SELL") << " "<< order.price << " "<< order.quantity<< "\n";

    out.close();
}

void Journaler::replayOrders() {

    std::ifstream in(filename);

    if(!in.is_open()) {

        std::cout<< "NOTE: No previous log found"<< std::endl;
        return;
    }

    std::cout
        << "\nReplaying Orders:"<< std::endl;

    std::string line;

    while(std::getline(in, line)) {

        std::cout << line << std::endl;
    }

    in.close();
}