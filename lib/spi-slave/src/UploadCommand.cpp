//
// Created by Adam Makswiej on 03/12/2023.
//

#include "../include/lib/spi/slave/UploadCommand.hpp"
#include "../include/lib/spi/slave/buffer/TypedInputBuffer.impl.hpp"

Lib::SPI::Slave::UploadCommand::UploadCommand(const ID id, Contract::FS *fs, const char *basepath)
: Command(id),  _size(0, &_size_buffer), _fs(fs, basepath), _data(0, &_fs) {
    _size.checksum(false);
    _size_buffer.validator([](const uint32_t size) -> bool {
        return size > 0;
    });
    _data.defer(true);
}

bool Lib::SPI::Slave::UploadCommand::expectedInput(Value value) {
    return _stage != FileCommandStage::Unknown;
}

Lib::SPI::Slave::Command::Response Lib::SPI::Slave::UploadCommand::handle(const bool initial, const Value value) {
    if (initial) {
        _stage = FileCommandStage::InitSize;
    }

    if (_stage == FileCommandStage::InitSize) {
        ++_stage;
        return passTo(&_size, true, value);
    }

    if (_stage == FileCommandStage::Size) {
        auto response = passTo(&_size, false, value);
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

    if (_stage == FileCommandStage::InitData) {
        ++_stage;
        _fs.open(destiantionPath(), _size_buffer.value());
        return passTo(&_data, true, value);
    }

    if (_stage == FileCommandStage::Data) {
        const auto response = passTo(&_data, false, value);
        if (!response.next) {
            ++_stage;
            if (response.value == OK) {
                _flag.execute = true;
            }
        }
        return response;
    }

    return Response{
        .value = BadCall,
        .next = false
    };
}

void Lib::SPI::Slave::UploadCommand::loop() {
    _fs.loop();
    if (_flag.execute) {
        _flag.execute = false;
        _fs.trigger();
    }
}

Lib::CRC::CRC16 Lib::SPI::Slave::UploadCommand::crc() {
    return _data.crc();
}

Lib::SPI::Slave::TypedBuffer<unsigned long> * Lib::SPI::Slave::UploadCommand::size() {
    return &_size_buffer;
}

Lib::SPI::Slave::UploadCommand * Lib::SPI::Slave::UploadCommand::grabDataBuffer(TypedBuffer<uint32_t> *&buffer) {
    buffer = &_size_buffer;
    return this;
}

Lib::SPI::Slave::UploadCommand * Lib::SPI::Slave::UploadCommand::withDataBuffer(const std::function<void(TypedBuffer<uint32_t> &buffer)> &fn) {
    fn(_size_buffer);
    return this;
}

Lib::SPI::Slave::FilesystemInputBuffer * Lib::SPI::Slave::UploadCommand::data() {
    return &_fs;
}

Lib::SPI::Slave::UploadCommand * Lib::SPI::Slave::UploadCommand::grabDataBuffer(FilesystemInputBuffer *&buffer) {
    buffer = &_fs;
    return this;
}

Lib::SPI::Slave::UploadCommand * Lib::SPI::Slave::UploadCommand::withDataBuffer(const std::function<void(FilesystemInputBuffer &buffer)> &fn) {
    fn(_fs);
    return this;
}
