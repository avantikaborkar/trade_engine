#include "journal/journaler.h"

#include <fstream>
#include <sstream>
#include <iostream>

Journaler::Journaler(
    const std::string& file
)
    : filename(file),
      running(true)
{
    writerThread = std::thread(
        &Journaler::writerLoop,
        this
    );
}

Journaler::~Journaler()
{
    running = false;

    queue.stop();

    if(writerThread.joinable())
    {
        writerThread.join();
    }
}

void Journaler::writerLoop()
{
    std::ofstream out(
        filename,
        std::ios::app
    );

    if(!out.is_open())
    {
        std::cout
            << "NOTE: Failed to open log file\n";

        return;
    }

    JournalEntry entry;

    while(queue.pop(entry))
    {
        out << entry.line;
    }

    out.flush();
}

void Journaler::logOrder(
    const Order& order
)
{
    std::ostringstream ss;

    ss
        << order.orderId << " "
        << order.symbol << " "
        << (order.side == Side::BUY ? "BUY" : "SELL")
        << " "
        << order.price << " "
        << order.quantity
        << "\n";

    queue.push(
        JournalEntry(
            ss.str()
        )
    );
}

void Journaler::replayOrders(
    OrderBook& book
)
{
    std::ifstream in(filename);

    if(!in.is_open())
    {
        return;
    }

    int id;
    std::string symbol;
    std::string sideStr;
    int price;
    int qty;

    while(
        in
        >> id
        >> symbol
        >> sideStr
        >> price
        >> qty
    )
    {
        Side side =
            (sideStr == "BUY")
            ? Side::BUY
            : Side::SELL;

        Order order(
            id,
            symbol,
            price,
            qty,
            side
        );

        book.addOrder(order);
    }
}

void Journaler::saveSnapshot(
    Exchange& exchange
)
{
    std::ofstream out(
        "data/snapshot.dat"
    );

    if(!out.is_open())
    {
        return;
    }

    const auto& books =
        exchange.getAllBooks();

    for(const auto& pair : books)
    {
        const std::string& symbol =
            pair.first;

        const OrderBook& book =
            pair.second;

        auto orders =
            book.getAllOrders();

        for(const auto& order : orders)
        {
            out
                << order.orderId << " "
                << symbol << " "
                << (order.side == Side::BUY
                    ? "BUY"
                    : "SELL")
                << " "
                << order.price << " "
                << order.quantity
                << "\n";
        }
    }

    std::cout
        << "[SNAPSHOT] Saved\n";
}

void Journaler::loadSnapshot(
    Exchange& exchange
)
{
    std::ifstream in(
        "data/snapshot.dat"
    );

    if(!in.is_open())
    {
        return;
    }

    int id;
    std::string symbol;
    std::string sideStr;
    int price;
    int qty;

    while(
        in
        >> id
        >> symbol
        >> sideStr
        >> price
        >> qty
    )
    {
        Side side =
            (sideStr == "BUY")
            ? Side::BUY
            : Side::SELL;

        Order order(
            id,
            symbol,
            price,
            qty,
            side
        );

        exchange
            .getBook(symbol)
            .addOrder(order);
    }
}

int Journaler::getHighestOrderId()
{
    std::ifstream in(
        "data/snapshot.dat"
    );

    if(!in.is_open())
    {
        return 0;
    }

    int maxId = 0;

    int id;
    std::string symbol;
    std::string sideStr;
    int price;
    int qty;

    while(
        in
        >> id
        >> symbol
        >> sideStr
        >> price
        >> qty
    )
    {
        if(id > maxId)
        {
            maxId = id;
        }
    }

    return maxId;
}