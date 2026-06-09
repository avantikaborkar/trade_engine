#include "network/tcp_server.h"

#include "network/binary_order_packet.h"

#include <iostream>

#ifdef _WIN32

#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#else

#include <unistd.h>
#include <arpa/inet.h>

#endif

TCPServer::TCPServer(
    int p,
    OrderGateway& gw
)
    : port(p),
      gateway(gw),
      running(false),
      serverSocket(-1) {}

void TCPServer::start() {

    running = true;

    serverThread = std::thread(
        &TCPServer::run,
        this
    );
}

void TCPServer::stop() {

    running = false;

#ifdef _WIN32
    closesocket(serverSocket);
    WSACleanup();
#else
    close(serverSocket);
#endif

    if(serverThread.joinable()) {

        serverThread.join();
    }
}

void TCPServer::run() {

#ifdef _WIN32

    WSADATA wsa;

    WSAStartup(
        MAKEWORD(2,2),
        &wsa
    );

#endif

    serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;

    serverAddr.sin_addr.s_addr = INADDR_ANY;

    serverAddr.sin_port = htons(port);

    bind(
        serverSocket,
        (sockaddr*)&serverAddr,
        sizeof(serverAddr)
    );

    listen(serverSocket, 10);

    std::cout
        << "[TCP] Multi-client server listening on port "
        << port
        << "\n";

    while(running) {

        sockaddr_in clientAddr;

#ifdef _WIN32
        int clientLen = sizeof(clientAddr);
#else
        socklen_t clientLen = sizeof(clientAddr);
#endif

        int clientSocket = accept(
            serverSocket,
            (sockaddr*)&clientAddr,
            &clientLen
        );

        if(clientSocket < 0) {

            continue;
        }

        std::cout
            << "[TCP] Client connected\n";

        std::thread(
            &TCPServer::handleClient,
            this,
            clientSocket
        ).detach();
    }
}

void TCPServer::handleClient(
    int clientSocket
) {

    while(true) {

        BinaryOrderPacket packet;

        char* packetPtr =
            reinterpret_cast<char*>(&packet);

        int totalReceived = 0;

        while(totalReceived < sizeof(packet)) {

#ifdef _WIN32

            int bytesReceived = recv(
                clientSocket,
                packetPtr + totalReceived,
                sizeof(packet) - totalReceived,
                0
            );

#else

            int bytesReceived = read(
                clientSocket,
                packetPtr + totalReceived,
                sizeof(packet) - totalReceived
            );

#endif

            if(bytesReceived <= 0) {

                totalReceived = -1;

                break;
            }

            totalReceived += bytesReceived;
        }

        if(totalReceived <= 0) {

            break;
        }

        std::cout
        << "[TCP] Binary Order -> "
        << "symbol="
        << packet.symbol
        << " side="
        << (int)packet.side
        << " type="
        << (int)packet.orderType
        << " price="
        << packet.price
        << " qty="
        << packet.quantity
        << "\n";

        gateway.receiveBinaryOrder(
            std::string(packet.symbol),
            packet.side,
            packet.orderType,
            packet.price,
            packet.quantity
        );
    }

#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif

    std::cout
        << "[TCP] Client disconnected\n";
}