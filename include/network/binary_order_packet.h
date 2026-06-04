#pragma once

#include <cstdint>

#pragma pack(push, 1)

struct BinaryOrderPacket {

    uint8_t side;

    int32_t price;

    int32_t quantity;
};

#pragma pack(pop)