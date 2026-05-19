#include "market_data/market_data_publisher.h"

#include <iostream>

MarketDataPublisher::MarketDataPublisher(SPSCQueue<TradeEvent>& queue): tradeQueue(queue), running(true) {}

void MarketDataPublisher::start() {

    std::thread(&MarketDataPublisher::publishLoop, this).detach();
}

void MarketDataPublisher::stop() 
{
    running = false;
}

void MarketDataPublisher::publishLoop() {

    TradeEvent event;

    while(running) 
    {

        if(tradeQueue.pop(event)) 
        {

            std::cout<< "TRADE "<< event.quantity<< " @ "<< event.price<< "\n";
        }
    }
}