//
// Created by Adam Makswiej on 02/12/2023.
//

#include "../include/lib/spi/slave/TransferInCommand.hpp"
#include "./helper.hpp"

Lib::SPI::Slave::TransferInCommand::TransferInCommand(const ID id, InputBuffer *buffer): InputCommand(id, buffer) {
    _flag.echo = true;
}

Lib::SPI::Slave::InputCommand * Lib::SPI::Slave::TransferInCommand::checksum(const bool checksum) {
    _flag.crc = checksum;
    return this;
}

void Lib::SPI::Slave::TransferInCommand::initTransfer() {
    _position = 0;
    InputCommand::initTransfer();
}

Lib::SPI::Slave::Buffer::BigSize Lib::SPI::Slave::TransferInCommand::bufferSize() const {
    return InputCommand::bufferSize() + 2 * (InputCommand::bufferSize() / _blockSize);
}

Lib::SPI::Slave::Command::Response Lib::SPI::Slave::TransferInCommand::handleData(const Buffer::Offset offset, const Buffer::Byte input) {
    if (const auto mod = offset % (_blockSize + 2); mod >= _blockSize) {
        if (mod == _blockSize) {
            return proceed(GoOn);
        }

        if (input > 0) {
            _position -= _blockSize;
            seek(offset - (_blockSize + 2));
            return proceed(Repeat);
        }

        return proceed(GoOn);
    }

    return InputCommand::handleData(_position++, input);
}

void Lib::SPI::Slave::TransferInCommand::blockSize(const Buffer::Size size) {
    _blockSize = size;
}

Lib::SPI::Slave::Buffer::Size Lib::SPI::Slave::TransferInCommand::blockSize() const {
    return _blockSize;
}
