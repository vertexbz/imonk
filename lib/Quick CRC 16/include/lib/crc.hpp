#ifndef __LIB_CRC_HPP__
#define __LIB_CRC_HPP__

#include <stdint.h>

namespace Lib::CRC {
    typedef uint16_t CRC16;

    constexpr CRC16 Initial16 = 0xFFFF;

    CRC16 crc16_step(uint8_t data, CRC16 last = Initial16);
    CRC16 crc16_quick(const uint8_t *data, uint16_t size);
}

#endif 
