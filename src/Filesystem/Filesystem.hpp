#pragma once

#include <FS.h>

namespace Filesystem {
class Filesystem : FS {
public:
    static size_t getFlashSize();
    using FS::exists;
    using FS::open;

    Filesystem();
    void begin();

    Dir images();
    Dir scenes();
};
}
