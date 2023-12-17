//
// Created by Adam Makswiej on 10/12/2023.
//

#include "Dir.hpp"
#include "Impl/Dir.hpp"
#include "util.hpp"

Filesystem::File Filesystem::Dir::openFile(const char *mode) {
    if (!_impl) {
        return File{};
    }

    OpenMode om;
    AccessMode am;
    if (!sflags(mode, om, am)) {
        return File{};
    }

    File f(_impl->openFile(om, am), _baseFS);
    f.setTimeCallback(_timeCallback);
    return f;
}

bool Filesystem::Dir::firstFile() {
    do {
        if (isFile()) {
            return true;
        }
    } while (next());

    return false;
}

bool Filesystem::Dir::nextFile() {
    while (next()) {
        if (isFile()) {
            return true;
        }
    }

    return false;
}

Lib::CRC::CRC16 Filesystem::Dir::fileCRC() const {
    Lib::CRC::CRC16 crc = 0;
    if (_impl && reinterpret_cast<Impl::Dir*>(_impl.get())->fileTag(File::TagCRC, &crc, sizeof(crc))) {
        return crc;
    }

    return Lib::CRC::Initial16;
}
