//
// Created by Adam Makswiej on 02/12/2023.
//

#include "../../include/lib/spi/slave/buffer/OutputBuffer.hpp"

void Lib::SPI::Slave::OutputBuffer::doneRedaing() {
    _read = true;
}

bool Lib::SPI::Slave::OutputBuffer::read() {
    const bool result = _read;
    _read = false;
    return result;
}
