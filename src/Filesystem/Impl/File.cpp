//
// Created by Adam Makswiej on 09/12/2023.
//

#include "./Filesystem.hpp"
#include "./File.hpp"


Filesystem::Impl::File::File(Filesystem *fs, const char *name, std::shared_ptr<lfs_file_t> fd, int flags, time_t creation)
: LittleFSFileImpl(fs, name, std::move(fd), flags, creation), _myfs(fs) {

}


bool Filesystem::Impl::File::setTag(Tag tag, const void* data, size_t size) {
    return lfs_setattr(_myfs->getRawFS(), _name.get(), tag, data, size) >= 0;
}

bool Filesystem::Impl::File::getTag(Tag tag, void* data, size_t size) {
    return lfs_getattr(_myfs->getRawFS(), _name.get(), tag, data, size) == size;
}
