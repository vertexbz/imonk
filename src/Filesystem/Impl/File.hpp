#pragma once

#include <LittleFS.h>

#include "Filesystem.hpp"

namespace Filesystem::Impl {
class Filesystem;

class File : public littlefs_impl::LittleFSFileImpl {
    Filesystem* _myfs;
public:
    typedef uint8_t Tag;

    File(Filesystem* fs, const char *name, std::shared_ptr<lfs_file_t> fd, int flags, time_t creation);

    bool getTag(Tag tag, void *data, size_t size);

    bool setTag(Tag tag, const void *data, size_t size);
};
}
