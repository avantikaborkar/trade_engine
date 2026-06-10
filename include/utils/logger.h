#pragma once

#include <mutex>
#include <iostream>

class Logger {

private:
    static std::mutex logMutex;

public:

    template<typename T>
    static void log(const T& msg) {

        std::lock_guard<std::mutex> lock(logMutex);

        std::cout << msg << std::endl;
    }
};