//
// Created by Adam Makswiej on 02/12/2023.
//

#include "../../include/lib/spi/slave/buffer/InputBuffer.hpp"

void Lib::SPI::Slave::InputBuffer::didChange() {
    _changed = true;
    // todo ensure integrity and double bufferring
}

bool Lib::SPI::Slave::InputBuffer::changed() {
    const bool changed = _changed;
    _changed = false;
    return changed;
}
