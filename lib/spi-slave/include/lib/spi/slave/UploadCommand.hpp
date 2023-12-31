//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__UPLOADCOMMAND_HPP__
#define __SPISLAVELIB__UPLOADCOMMAND_HPP__

#include "./contract/filesytem.hpp"
#include "./buffer/FilesystemInputBuffer.hpp"
#include "./buffer/TypedInputBuffer.hpp"
#include "./InputCommand.hpp"
#include "./TransferInCommand.hpp"
#include "./FileCommandStage.hpp"

namespace Lib::SPI::Slave {
class UploadCommand : public Command {
protected:
    TypedInputBuffer<uint32_t> _size_buffer{};
    InputCommand _size;
    FilesystemInputBuffer _fs;
    TransferInCommand _data;

    FileCommandStage _stage = FileCommandStage::Unknown;

    volatile struct {
        volatile bool execute = false;
    } _flag{};

    bool expectedInput(Value value) override;

    Response handle(bool initial, Value value) override;

    virtual const char *destiantionPath() = 0;

    CRC::CRC16 crc();

public:
    explicit UploadCommand(ID id, Contract::FS *fs, const char *basepath = "/");

    TypedBuffer<uint32_t> *size();

    UploadCommand *grabDataBuffer(TypedBuffer<uint32_t> *&buffer);

    UploadCommand *withDataBuffer(const std::function<void(TypedBuffer<uint32_t> &buffer)> &fn);

    FilesystemInputBuffer *data();

    UploadCommand *grabDataBuffer(FilesystemInputBuffer *&buffer);

    UploadCommand *withDataBuffer(const std::function<void(FilesystemInputBuffer &buffer)> &fn);

    void loop() override;

};
}

#endif // __SPISLAVELIB__UPLOADCOMMAND_HPP__
