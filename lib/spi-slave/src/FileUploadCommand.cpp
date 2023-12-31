//
// Created by Adam Makswiej on 03/12/2023.
//

#include "../include/lib/spi/slave/FileUploadCommand.hpp"

Lib::SPI::Slave::FileUploadCommand::FileUploadCommand(const ID id, Contract::FS *fs, const char *basepath)
: UploadCommand(id, fs, basepath), _name(id, &_name_buffer) {
    _name.checksum(false);
    _name_buffer.validator([](const Buffer::Data data, const Buffer::Size size) -> bool {
        return size > 0 && data[size] == 0;
    });
}

Lib::SPI::Slave::Command::Response Lib::SPI::Slave::FileUploadCommand::handle(const bool initial, const Value value) {
    if (initial) {
        _stage = FileCommandStage::Name;
    }
    if (_stage == FileCommandStage::Name) {
        auto response = passTo(&_name, initial, value);
        if (!response.next) {
            response.next = true;
            if (response.value == OK) {
                ++_stage;
                response.value = ok;
            } else {
                _stage = FileCommandStage::Unknown;
            }
        }
        return response;
    }

    return UploadCommand::handle(initial, value);
}

const char * Lib::SPI::Slave::FileUploadCommand::destiantionPath() {
    return reinterpret_cast<char*>(_name_buffer.value());
}

Lib::SPI::Slave::VariableInputBuffer * Lib::SPI::Slave::FileUploadCommand::name() {
    return &_name_buffer;
}

Lib::SPI::Slave::FileUploadCommand * Lib::SPI::Slave::FileUploadCommand::grabNameBuffer(VariableInputBuffer *&buffer) {
    buffer = &_name_buffer;
    return this;
}

Lib::SPI::Slave::FileUploadCommand * Lib::SPI::Slave::FileUploadCommand::withNameBuffer(const std::function<void(VariableInputBuffer & buffer)> &fn) {
    fn(_name_buffer);
    return this;
}
