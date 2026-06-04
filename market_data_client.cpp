#include <iostream>

#ifdef _WIN32

#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#else

#include <unistd.h>
#include <arpa/inet.h>

#endif

int main() {

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

    sockaddr_in server;

    server.sin_family = AF_INET;

    server.sin_port = htons(9000);

    server.sin_addr.s_addr = INADDR_ANY;

    bind(
        sock,
        (sockaddr*)&server,
        sizeof(server)
    );

    std::cout
        << "Listening for market data...\n";

    char buffer[1024];

    while(true) {

        int bytesReceived = recvfrom(
            sock,
            buffer,
            sizeof(buffer) - 1,
            0,
            nullptr,
            nullptr
        );

        if(bytesReceived > 0) {

            buffer[bytesReceived] = '\0';

            std::cout
                << buffer
                << "\n";
        }
    }

#ifdef _WIN32

    closesocket(sock);

    WSACleanup();

#else

    close(sock);

#endif

    return 0;
}