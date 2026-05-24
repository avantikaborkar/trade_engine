#pragma once

#include "gateway/order_gateway.h"

#include <thread>
#include <atomic>

class TCPServer {

private:
    int serverSocket;

    int port;

    std::atomic<bool> running;

    std::thread serverThread;

    OrderGateway& gateway;

public:
    TCPServer(int p, OrderGateway& gw);

    void start();

    void stop();

private:
    void run();
};