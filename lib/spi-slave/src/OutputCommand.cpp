//
// Created by Adam Makswiej on 22/05/2022.
//

#include "../include/lib/spi/slave/OutputCommand.hpp"
#include <lib/crc.hpp>
#include "./helper.hpp"

Lib::SPI::Slave::OutputCommand::OutputCommand(const ID id, OutputBuffer *buffer) : Command(id), _buffer(buffer) {}

Lib::SPI::Slave::OutputCommand::~OutputCommand() {
    delete _buffer;
}

Lib::SPI::Slave::Command::Response
Lib::SPI::Slave::OutputCommand::handle(const bool initial, Value value) {
    if (initial) { // read
        _position = 0;
        _crc = CRC::Initial16;
    }

    const auto buffer_size = _buffer->size();
    if (_position < buffer_size) {
        const auto val = _buffer->get(_position++);
        _crc = CRC::crc16_step(val, _crc);
        return proceed(val);
    }

    if (_position < buffer_size + sizeof(_crc)) {
        const auto val = reinterpret_cast<Buffer::Data>(const_cast<uint16_t*>(&_crc))[_position++ - buffer_size];
        if (_position < buffer_size + sizeof(_crc)) {
            return proceed(val);
        }

        _buffer->doneRedaing();
        _position = 0;

        if (_flag.deferred) {
            _flag.execute = true;
        } else {
            _buffer->trigger();
        }

        return stop(val);
    }

    return stop(BadCall);
}

bool Lib::SPI::Slave::OutputCommand::expectedInput(const Value value) {
    return value == 0;
}

Lib::SPI::Slave::OutputBuffer * Lib::SPI::Slave::OutputCommand::buffer() const {
    return _buffer;
}

void Lib::SPI::Slave::OutputCommand::loop() {
    if (_flag.deferred && _flag.execute) {
        _flag.execute = false;
        _buffer->trigger();
    }
}

Lib::SPI::Slave::OutputCommand *Lib::SPI::Slave::OutputCommand::defer(const bool deferred) {
    _flag.deferred = deferred;
    return this;
}