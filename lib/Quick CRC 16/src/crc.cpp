//
// Created by Adam Makswiej on 14/02/2022.
//

#include <lib/crc.hpp>

Lib::CRC::CRC16 Lib::CRC::crc16_step(const uint8_t data, CRC16 last) {
    uint8_t x = last >> 8 ^ data;
    x ^= x>>4;
    return (last << 8) ^ static_cast<uint16_t>(x << 12) ^ static_cast<uint16_t>(x << 5) ^ static_cast<uint16_t>(x);
}

Lib::CRC::CRC16 Lib::CRC::crc16_quick(const uint8_t *data, uint16_t size) {
    if (data == nullptr) {
        return 0;
    }

    CRC16 crc = Initial16;
    while (size--) {
        crc = crc16_step(*data++, crc);
    }
    return crc;
}
