#pragma once

#include <FS.h>

namespace Filesystem {
class Filesystem : public FS {
public:
    static size_t getFlashSize();

    Filesystem();

    void begin();

    Dir images();
    Dir scenes();
};
}
