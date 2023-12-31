//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__FILEUPLOADCOMMAND_HPP__
#define __SPISLAVELIB__FILEUPLOADCOMMAND_HPP__

#include "./buffer/VariableInputBuffer.hpp"
#include "./UploadCommand.hpp"

namespace Lib::SPI::Slave {
class FileUploadCommand : public UploadCommand {
protected:
    VariableInputBuffer _name_buffer{};
    InputCommand _name;

    Response handle(bool initial, Value value) override;

    const char * destiantionPath() override;

public:
    FileUploadCommand(ID id, Contract::FS *fs, const char *basepath);

    VariableInputBuffer *name();

    FileUploadCommand *grabNameBuffer(VariableInputBuffer *&buffer);

    FileUploadCommand *withNameBuffer(const std::function<void(VariableInputBuffer &buffer)> &fn);
};
}

#endif // __SPISLAVELIB__FILEUPLOADCOMMAND_HPP__
