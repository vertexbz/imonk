#pragma once

#include "../Filesystem/Filesystem.hpp"
#include <vertaro/spi/slave.hpp>


namespace Communication {
class CRCFileUploadCommand : public Lib::SPI::Slave::FileUploadCommand {
public:
    CRCFileUploadCommand(ID id, Filesystem::Filesystem *fs, const char *basepath);
};
}
