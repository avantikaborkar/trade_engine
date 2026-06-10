#include "market_data/market_data_publisher.h"
#include "utils/logger.h"
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
    SPSCQueue<TradeEvent>& queue
)
    : tradeQueue(queue),
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
                event.symbol +
                " TRADE " +
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

            Logger::log(
                "[UDP] " + tradeMsg
            );
        }
    }

#ifdef _WIN32

    closesocket(sock);

    WSACleanup();

#else

    close(sock);

#endif
}