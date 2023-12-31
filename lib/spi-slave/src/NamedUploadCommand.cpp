//
// Created by Adam Makswiej on 03/12/2023.
//

#include "../include/lib/spi/slave/NamedUploadCommand.hpp"

Lib::SPI::Slave::NamedUploadCommand::NamedUploadCommand(const ID id, Contract::FS *fs, char *filepath)
: UploadCommand(id, fs, ""), _filepath(strdup(filepath)) {

}
Lib::SPI::Slave::NamedUploadCommand::NamedUploadCommand(const ID id, Contract::FS *fs, const char *filepath)
: UploadCommand(id, fs, ""), _filepath(filepath) {

}

Lib::SPI::Slave::NamedUploadCommand::~NamedUploadCommand() {
    delete _filepath;
}

const char * Lib::SPI::Slave::NamedUploadCommand::destiantionPath() {
    return _filepath;
}
