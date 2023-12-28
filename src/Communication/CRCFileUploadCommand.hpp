#pragma once

#include "../Filesystem/Filesystem.hpp"
#include <vertaro/spi/slave.hpp>

namespace Filesystem{
class UploadFile;
}

namespace Communication {
class CRCFileUploadCommand : public Lib::SPI::Slave::FileUploadCommand {
public:
    typedef std::function<void(Filesystem::UploadFile* file)> Callback;
private:
    Callback _callback;
public:
    CRCFileUploadCommand(ID id, Filesystem::Filesystem *fs, const char *basepath);
    void callback(const Callback &callback);
};
}
