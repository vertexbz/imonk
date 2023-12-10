#pragma once

#include "Dir.hpp"
#include "File.hpp"

namespace Filesystem {
class Filesystem : public FS {
public:
    static size_t getFlashSize();

    Filesystem();

    void begin();

    Dir images();

    Dir scenes();

    Dir openDir(const String& path);

    Dir openDir(const char* path);

    File open(const String& path, const char* mode);

    File open(const char* path, const char* mode);
};
}
