//
// Created by Adam Makswiej on 16/12/2023.
//

#include "./CRCFileUploadCommand.hpp"

#include <Filesystem/UploadFile.hpp>

Communication::CRCFileUploadCommand::CRCFileUploadCommand(ID id, Filesystem::Filesystem *fs, const char *basepath): FileUploadCommand(id, fs, basepath) {
    _fs.callback([this](Lib::SPI::Slave::Contract::FS::File *file) {
        static_cast<Filesystem::UploadFile*>(file)->setCRC(crc());
    });
}
