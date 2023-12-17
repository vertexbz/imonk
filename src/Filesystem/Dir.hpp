#pragma once

#include "File.hpp"
#include <vertaro/crc.hpp>
#include <FS.h>

namespace Filesystem {
class Dir : public fs::Dir {
public:
    using fs::Dir::Dir;
    virtual ~Dir() = default;

    File openFile(const char* mode);

    bool firstFile();

    bool nextFile();

    [[nodiscard]] Lib::CRC::CRC16 fileCRC() const;

};
}
