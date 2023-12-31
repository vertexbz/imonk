//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__SPISLAVE_COMMAND_TRANSFER_IN_HPP__
#define __SPISLAVELIB__SPISLAVE_COMMAND_TRANSFER_IN_HPP__

#include "InputCommand.hpp"

namespace Lib::SPI::Slave {
class FileUploadCommand;

class TransferInCommand : public InputCommand {
    volatile uintptr_t _position = 0; // actual data offset
protected:
    Buffer::Size _blockSize = 16;

    void initTransfer() override;

    [[nodiscard]] Buffer::BigSize bufferSize() const override;

    Response handleData(Buffer::Offset offset, Buffer::Byte input) override;

public:
    TransferInCommand(ID id, InputBuffer *buffer);

    InputCommand * checksum(bool checksum) override;

    void blockSize(Buffer::Size size);

    [[nodiscard]] Buffer::Size blockSize() const;
};
}

#endif // __SPISLAVELIB__SPISLAVE_COMMAND_TRANSFER_IN_HPP__
