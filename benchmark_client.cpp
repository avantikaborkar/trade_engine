#include <winsock2.h>
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

#pragma pack(push, 1)

struct BinaryOrderPacket {

    char symbol[16];

    uint8_t side;

    uint8_t orderType;

    int32_t price;

    int32_t quantity;
};

#pragma pack(pop)

int main() {

    WSADATA wsa;

    WSAStartup(
        MAKEWORD(2,2),
        &wsa
    );

    SOCKET sock = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr =
        inet_addr("127.0.0.1");

    if(
        connect(
            sock,
            (sockaddr*)&server,
            sizeof(server)
        ) != 0
    ) {
        std::cout
            << "Connection failed\n";

        return 1;
    }

    BinaryOrderPacket packet;

    memset(
        &packet,
        0,
        sizeof(packet)
    );

    const char* symbols[] =
    {
        "AAPL",
        "MSFT",
        "GOOG",
        "AMZN",
        "META",
        "TSLA",
        "NVDA"
    };

    packet.orderType = 1;   // LIMIT
    packet.price = 500;
    packet.quantity = 1;

    const int NUM_ORDERS = 5000000;

    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < NUM_ORDERS;++i) {


        strcpy(
        packet.symbol,
        symbols[i % 7]
    );
        // Alternate BUY and SELL
        packet.side =
            (i % 2 == 0)
            ? 1   // BUY
            : 2;  // SELL

        send(
            sock,
            reinterpret_cast<char*>(&packet),
            sizeof(packet),
            0
        );
    }

    auto end =
    std::chrono::steady_clock::now();

    double seconds =
        std::chrono::duration<double>(
            end - start
        ).count();

    std::cout
        << "Orders/sec: "
        << NUM_ORDERS / seconds
        << "\n";
    std::cout
        << "Sent "
        << NUM_ORDERS
        << " orders\n";

    closesocket(sock);

    WSACleanup();

    return 0;
}