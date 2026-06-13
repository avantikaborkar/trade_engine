#pragma once

#include <string>

struct JournalEntry {

    std::string line;

    JournalEntry() = default;

    JournalEntry(
        const std::string& l
    )
        : line(l) {}
};