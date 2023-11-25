#pragma once

#include <FS.h>

namespace Filesystem {
class Filesystem : private FS {
public:
    static size_t getFlashSize();

    Filesystem();
    void begin();

    Dir images();
    Dir scenes();
};
}
