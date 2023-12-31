//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__NAMEDUPLOADCOMMAND_HPP__
#define __SPISLAVELIB__NAMEDUPLOADCOMMAND_HPP__

#include "./UploadCommand.hpp"

namespace Lib::SPI::Slave {
class NamedUploadCommand : public UploadCommand {
protected:
    const char *_filepath;
    const char * destiantionPath() override;

public:
    NamedUploadCommand(ID id, Contract::FS *fs, char *filepath);
    NamedUploadCommand(ID id, Contract::FS *fs, const char *filepath);

    ~NamedUploadCommand() override;
};
}

#endif // __SPISLAVELIB__NAMEDUPLOADCOMMAND_HPP__
