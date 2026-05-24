#include "network/tcp_server.h"

#include <iostream>
#include <cstring>

#ifdef _WIN32

#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#else

#include <unistd.h>
#include <arpa/inet.h>

#endif

TCPServer::TCPServer(int p, OrderGateway& gw) : port(p), gateway(gw), running(false), serverSocket(-1) {}

void TCPServer::start() {

    running = true;

    serverThread = std::thread( &TCPServer::run, this);
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

    WSAStartup( MAKEWORD(2,2), &wsa);

#endif

    serverSocket = socket(AF_INET,SOCK_STREAM,0);

    if(serverSocket < 0) {

        std::cout << "[TCP] Socket creation failed\n";

        return;
    }

    sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;

    serverAddr.sin_addr.s_addr = INADDR_ANY;

    serverAddr.sin_port = htons(port);

    if(bind(serverSocket,(sockaddr*)&serverAddr,sizeof(serverAddr)) < 0) {

        std::cout<< "[TCP] Bind failed\n";

        return;
    }

    listen(serverSocket, 5);

    std::cout << "[TCP] Server listening on port " << port << "\n";

    while(running) {

        sockaddr_in clientAddr;

#ifdef _WIN32
        int clientLen = sizeof(clientAddr);
#else
        socklen_t clientLen = sizeof(clientAddr);
#endif

        int clientSocket = accept( serverSocket, (sockaddr*)&clientAddr, &clientLen);

        if(clientSocket < 0) 
        {
            continue;
        }

        std::cout<< "[TCP] Client connected\n";

        char buffer[1024];

std::string messageBuffer;

while(true) {

#ifdef _WIN32

    int bytesReceived = recv(
        clientSocket,
        buffer,
        sizeof(buffer)-1,
        0
    );

#else

    int bytesReceived = read(
        clientSocket,
        buffer,
        sizeof(buffer)-1
    );

#endif

    if(bytesReceived <= 0) {
        break;
    }

    buffer[bytesReceived] = '\0';

    messageBuffer += buffer;

    size_t pos;

    while((pos = messageBuffer.find('\n'))!= std::string::npos) {

        std::string message =
            messageBuffer.substr(0, pos);

        messageBuffer.erase(0, pos + 1);

        if(!message.empty() &&
           message.back() == '\r') {

            message.pop_back();
        }

        std::cout << "[TCP] Received: " << message << "\n";

        gateway.receiveMessage(message);
    }
}

#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif

        std::cout
            << "[TCP] Client disconnected\n";
    }
}