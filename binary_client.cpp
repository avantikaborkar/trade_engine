#include <iostream>
#include <winsock2.h>
#include <cstdint>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#pragma pack(push, 1)

struct BinaryOrderPacket {

    uint8_t side;

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

    int sock = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    sockaddr_in server;

    server.sin_family = AF_INET;

    server.sin_port = htons(8080);

    server.sin_addr.s_addr =
        inet_addr("127.0.0.1");

    if(connect(
        sock,
        (sockaddr*)&server,
        sizeof(server)
    ) < 0) {

        std::cout
            << "Connection failed\n";

        return 1;
    }

    BinaryOrderPacket packet;

    std::string side;

    std::cout
        << "Side (BUY/SELL): ";

    std::cin >> side;

    if(side == "BUY") {

        packet.side = 1;
    }
    else if(side == "SELL") {

        packet.side = 2;
    }
    else {

        std::cout
            << "Invalid side\n";

        return 1;
    }

    std::cout
        << "Price: ";

    std::cin >> packet.price;

    std::cout
        << "Quantity: ";

    std::cin >> packet.quantity;

    send(
        sock,
        (char*)&packet,
        sizeof(packet),
        0
    );

    std::cout
        << "Binary order sent\n";

    closesocket(sock);

    WSACleanup();

    return 0;
}