#pragma once

#include "File.hpp"
#include <LittleFS.h>

namespace Filesystem::Impl {
class Dir : public littlefs_impl::LittleFSDirImpl {
public:
    using LittleFSDirImpl::LittleFSDirImpl;
    using Tag = File::Tag;

    bool fileTag(Tag tag, void *data, size_t size);
};
}
