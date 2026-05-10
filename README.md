Low Latency Trading Engine
This project is a basic Trading Engine implemented in C++.
It simulates how financial markets process buy and sell orders using an order matching system.
The engine matches orders based on price and time priority, similar to real-world stock exchanges.

Key Concepts
- Order Book Management
- Price-Time Priority Matching
- Buy/Sell Order Processing
- Data Structures (Priority Queue / Map)
- Object-Oriented Programming (C++)

Core Technologies
- C++ (STL + Modern C++)
- Lock-free SPSC Queue (Single Producer Single Consumer)
- Ring Buffer for efficient data handling
- Custom Memory Pool for fast allocation/deallocation

Future Improvements
- Support for Market Orders
- Multi-threaded order processing
- Real-time data feed integration
- GUI visualization
- Risk management system
