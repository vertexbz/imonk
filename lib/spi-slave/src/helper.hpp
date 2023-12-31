//
// Created by Adam Makswiej on 30/11/2023.
//


#ifndef __LIB_SPI_SLAVE_SRC_HELPER__
#define __LIB_SPI_SLAVE_SRC_HELPER__

#define respond(val, nxt) Response{\
    .value = static_cast<Value>(val),\
    .next = nxt\
}

#define stop(val) respond(val, false)
#define proceed(val) respond(val, true)
#define crc_proceed(input) respond(_flag.crc ? GoOn : input, true)

#endif // __LIB_SPI_SLAVE_SRC_HELPER__
