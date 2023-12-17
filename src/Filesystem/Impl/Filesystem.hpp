#pragma once

#include <LittleFS.h>

namespace Filesystem::Impl {
class Filesystem : public littlefs_impl::LittleFSImpl {
public:
    using LittleFSImpl::LittleFSImpl;
    lfs_t *getRawFS();

    FileImplPtr open(const char *path, OpenMode openMode, AccessMode accessMode) override;

    DirImplPtr openDir(const char *path) override;
};
}
