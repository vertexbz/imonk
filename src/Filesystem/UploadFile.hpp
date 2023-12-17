#pragma once

#include "./File.hpp"

namespace Filesystem {
class Filesystem;

class UploadFile : public File {
    const char *_path;
    bool _completed = false;
    bool _got_crc = false;
    Lib::CRC::CRC16 _crc = Lib::CRC::Initial16;
public:
    explicit UploadFile(FileImplPtr p = FileImplPtr(), Filesystem *baseFS = nullptr, const char *path = nullptr);

    ~UploadFile() override;

    void setCRC(Lib::CRC::CRC16 checksum) override;

    void completed() override;
};
}
