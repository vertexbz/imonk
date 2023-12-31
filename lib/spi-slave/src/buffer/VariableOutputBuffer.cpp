//
// Created by Adam Makswiej on 02/12/2023.
//

#include "../../include/lib/spi/slave/Command.hpp"
#include "../../include/lib/spi/slave/buffer/VariableOutputBuffer.hpp"

Lib::SPI::Slave::Buffer::Byte Lib::SPI::Slave::VariableOutputBuffer::get(Offset offset) const {
    if (offset < sizeof(_size)) {
        return reinterpret_cast<Data>(const_cast<Size *>(&_size))[offset];
    }
    offset -= sizeof(_size);
    if (offset < _size) {
        return _data[offset];
    }

    return Command::BadCall;
}

void Lib::SPI::Slave::VariableOutputBuffer::doneRedaing() {
    OutputBuffer::doneRedaing();
    // todo ensure integrity and double bufferring
}

void Lib::SPI::Slave::VariableOutputBuffer::update(Data data, const Size size) {
    _data = data;
    _size = size;
}
