//
// Created by Adam Makswiej on 02/12/2023.
//

#include "../../include/lib/spi/slave/buffer/VariableBuffer.hpp"

Lib::SPI::Slave::VariableBuffer::~VariableBuffer() {
    delete _data;
}

Lib::SPI::Slave::Buffer::BigSize Lib::SPI::Slave::VariableBuffer::size() const {
    return sizeof(_size) + _size;
}

void Lib::SPI::Slave::VariableBuffer::trigger() {
    _SPIS_CB_IF_CALL(callback, context, _data, _size);
}

_SPIS_CB_CTX_DEF(Lib::SPI::Slave::VariableBuffer, callback, context, Callback)

const Lib::SPI::Slave::Buffer::Data Lib::SPI::Slave::VariableBuffer::value() const {
    return _data;
}

const Lib::SPI::Slave::Buffer::Size Lib::SPI::Slave::VariableBuffer::length() const {
    return _size;
}
