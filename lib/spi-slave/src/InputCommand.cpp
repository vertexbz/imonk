//
// Created by Adam Makswiej on 30/11/2023.
//

#include "../include/lib/spi/slave/InputCommand.hpp"
#include <lib/crc.hpp>
#include "./helper.hpp"

Lib::SPI::Slave::InputCommand::InputCommand(const ID id, InputBuffer *buffer) : Command(id), _buffer(buffer) {}

Lib::SPI::Slave::InputCommand::~InputCommand() {
    delete _buffer;
}


Lib::SPI::Slave::Buffer::BigSize Lib::SPI::Slave::InputCommand::bufferSize() const {
    return _buffer->size();
}

void Lib::SPI::Slave::InputCommand::seek(const Buffer::Offset offset) {
    _position = offset;
}

void Lib::SPI::Slave::InputCommand::initTransfer() {
    _crc = CRC::Initial16;
    _crc_computed = CRC::Initial16;
    _buffer->reset();
}

Lib::SPI::Slave::Command::Response
Lib::SPI::Slave::InputCommand::handleData(const Buffer::Offset offset, const Buffer::Byte input) {
    _buffer->set(offset, input);
    if (offset == 0) {
        _top_position = 0;
        _crc_computed = CRC::crc16_step(input, _crc_computed);
    } else if (offset > _top_position) {
        _crc_computed = CRC::crc16_step(input, _crc_computed);
        _top_position = offset;
    }
    return proceed(_flag.echo ? input : GoOn);
}

Lib::SPI::Slave::Command::Response
Lib::SPI::Slave::InputCommand::handle(const bool initial, const Value input) {
    if (initial) {
        _position = 0;
        initTransfer();
        return proceed(GoOn);
    }

    const auto buffer_size = bufferSize();
    if (_position < buffer_size) {
        return handleData(_position++, input);
    }

    if (_flag.crc && _position < buffer_size + sizeof(_crc)) {
        const auto buffer = reinterpret_cast<Buffer::Data>(const_cast<uint16_t *>(&_crc));
        buffer[_position++ - buffer_size] = input;

        if (_position < buffer_size + sizeof(_crc)) {
            return proceed(GoOn);
        }
    }

    if (_position - buffer_size == (_flag.crc ? sizeof(_crc) : 0)) {
        if (_flag.crc && _crc != _crc_computed) {
            return stop(BadCRC);
        }

        const bool valid = _buffer->validate();
        if (valid) {
            _buffer->didChange();
            if (_flag.deferred) {
                _flag.execute = true;
            } else {
                _buffer->trigger();
            }
        }

        _position++;
        return stop(valid ? OK : Invalid);
    }

    return stop(BadCall);
}

bool Lib::SPI::Slave::InputCommand::expectedInput(Value value) {
    return _position < bufferSize() + (_flag.crc ? sizeof(_crc) : 1);
}

Lib::SPI::Slave::InputBuffer * Lib::SPI::Slave::InputCommand::buffer() const {
    return _buffer;
}

Lib::SPI::Slave::InputCommand *Lib::SPI::Slave::InputCommand::checksum(const bool checksum) {
    _flag.crc = checksum;
    _flag.echo = !checksum;
    return this;
}

void Lib::SPI::Slave::InputCommand::loop() {
    if (_flag.deferred && _flag.execute) {
        _flag.execute = false;
        _buffer->trigger();
    }
}

Lib::SPI::Slave::InputCommand *Lib::SPI::Slave::InputCommand::defer(const bool deferred) {
    _flag.deferred = deferred;
    return this;
}

Lib::CRC::CRC16 Lib::SPI::Slave::InputCommand::crc() {
    return _crc_computed;
}
