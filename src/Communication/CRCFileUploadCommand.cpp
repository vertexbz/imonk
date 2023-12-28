//
// Created by Adam Makswiej on 16/12/2023.
//

#include "./CRCFileUploadCommand.hpp"

#include <Filesystem/UploadFile.hpp>

Communication::CRCFileUploadCommand::CRCFileUploadCommand(const ID id, Filesystem::Filesystem *fs, const char *basepath)
: FileUploadCommand(id, fs, basepath) {
    _fs.callback([this](Lib::SPI::Slave::Contract::FS::File *file) {
        static_cast<Filesystem::UploadFile*>(file)->setCRC(crc());
        if (_callback) _callback(static_cast<Filesystem::UploadFile*>(file));
    });
}

void Communication::CRCFileUploadCommand::callback(const Callback &callback) {
    _callback = callback;
}
