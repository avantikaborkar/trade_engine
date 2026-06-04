#include "market_data/market_data_publisher.h"

#include <iostream>
#include <string>

#ifdef _WIN32

#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#else

#include <unistd.h>
#include <arpa/inet.h>

#endif

MarketDataPublisher::MarketDataPublisher(
    SPSCQueue<TradeEvent>& queue,
    OrderBook& book
)
    : tradeQueue(queue),
      orderBook(book),
      running(true) {}

void MarketDataPublisher::start() {

    std::thread(
        &MarketDataPublisher::publishLoop,
        this
    ).detach();
}

void MarketDataPublisher::stop() {

    running = false;
}

void MarketDataPublisher::publishLoop() {

#ifdef _WIN32

    WSADATA wsa;

    WSAStartup(
        MAKEWORD(2,2),
        &wsa
    );

#endif

    int sock = socket(
        AF_INET,
        SOCK_DGRAM,
        0
    );

    sockaddr_in clientAddr;

    clientAddr.sin_family = AF_INET;

    clientAddr.sin_port = htons(9000);

    clientAddr.sin_addr.s_addr =
        inet_addr("127.0.0.1");

    TradeEvent event;

    while(running) {

        if(tradeQueue.pop(event)) {

            std::string tradeMsg =
            "TRADE " +
            std::to_string(event.quantity) +
            " @ " +
            std::to_string(event.price);

            sendto(
                sock,
                tradeMsg.c_str(),
                (int)tradeMsg.size(),
                0,
                (sockaddr*)&clientAddr,
                sizeof(clientAddr)
            );

            std::cout
                << "[UDP] "
                << tradeMsg
                << "\n";

            std::string bidMsg =
                "BEST_BID " +
                std::to_string(
                    orderBook.getBestBid()
                );

            sendto(
                sock,
                bidMsg.c_str(),
                (int)bidMsg.size(),
                0,
                (sockaddr*)&clientAddr,
                sizeof(clientAddr)
            );

            std::string askMsg =
                "BEST_ASK " +
                std::to_string(
                    orderBook.getBestAsk()
                );

            sendto(
                sock,
                askMsg.c_str(),
                (int)askMsg.size(),
                0,
                (sockaddr*)&clientAddr,
                sizeof(clientAddr)
            );

            std::cout
                << "[UDP] "
                << bidMsg
                << "\n";

            std::cout
                << "[UDP] "
                << askMsg
                << "\n";
        }
    }

#ifdef _WIN32

    closesocket(sock);

    WSACleanup();

#else

    close(sock);

#endif
}