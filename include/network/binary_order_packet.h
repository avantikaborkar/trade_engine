#pragma once

#include <cstdint>

#pragma pack(push, 1)

struct BinaryOrderPacket {

    char symbol[16];

    uint8_t side;

    uint8_t orderType;

    int32_t price;

    int32_t quantity;
};

#pragma pack(pop)