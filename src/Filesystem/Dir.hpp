#pragma once

#include "File.hpp"
#include <FS.h>

namespace Filesystem {
class Dir : public fs::Dir {
public:
    using fs::Dir::Dir;
    virtual ~Dir() = default;

    File openFile(const char* mode);
};
}
