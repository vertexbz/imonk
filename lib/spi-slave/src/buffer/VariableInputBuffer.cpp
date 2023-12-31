//
// Created by Adam Makswiej on 02/12/2023.
//

#include "../../include/lib/spi/slave/buffer/VariableInputBuffer.hpp"

void Lib::SPI::Slave::VariableInputBuffer::set(Offset offset, const Byte value) {
    if (offset < sizeof(_size)) {
        reinterpret_cast<Data>(const_cast<Size *>(&_size))[offset] = value;
        if (offset == sizeof(_size) - 1) {
            delete[] _data; // todo ensure integrity and double bufferring
            _data = new Byte[_size + 1];
            _data[_size] = 0;
        }
        return;
    }

    offset -= sizeof(_size);
    if (offset < _size) {
        _data[offset] = value;
    }
}

bool Lib::SPI::Slave::VariableInputBuffer::validate() {
    if (!_validator) {
        return true;
    }

    return _SPIS_CB_CALL(validator, context_validator, _data, _size);
}

void Lib::SPI::Slave::VariableInputBuffer::reset() {
    delete[] _data;
    _data = nullptr;
}

_SPIS_CB_CTX_DEF(Lib::SPI::Slave::VariableInputBuffer, validator, context_validator, Validator)
