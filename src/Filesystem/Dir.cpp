//
// Created by Adam Makswiej on 10/12/2023.
//

#include "Dir.hpp"
#include "util.hpp"

Filesystem::File Filesystem::Dir::openFile(const char *mode) {
    if (!_impl) {
        return File{};
    }

    fs::OpenMode om;
    fs::AccessMode am;
    if (!sflags(mode, om, am)) {
        return File{};
    }

    File f(_impl->openFile(om, am), _baseFS);
    f.setTimeCallback(_timeCallback);
    return f;
}
