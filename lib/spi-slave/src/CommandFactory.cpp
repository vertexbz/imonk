//
// Created by Adam Makswiej on 03/12/2023.
//

#include "../include/lib/spi/slave/CommandFactory.hpp"

Lib::SPI::Slave::OutputCommand *Lib::SPI::Slave::CommandFactory::VariableOutputCommand(
        const Command::ID id,
        const VariableOutputBuffer::Callback &callback,
        const bool deferred
) {
    const auto buffer = new VariableOutputBuffer();
    buffer->callback(callback);
    return (new OutputCommand(id, buffer))->defer(deferred);
}

Lib::SPI::Slave::InputCommand *Lib::SPI::Slave::CommandFactory::VariableInputCommand(
        const Command::ID id,
        const VariableInputBuffer::Callback &callback,
        const bool deferred,
        const VariableInputBuffer::Validator &validator,
        const bool checksum
) {
    const auto buffer = new VariableInputBuffer();
    buffer->callback(callback);
    buffer->validator(validator);
    return (new InputCommand(id, buffer))->checksum(checksum)->defer(deferred);
}
