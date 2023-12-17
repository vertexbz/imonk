//
// Created by Adam Makswiej on 20/11/2023.
//

#include "Filesystem.hpp"
#include <hardware/flash.h>
#include "Impl/Filesystem.hpp"
#include "UploadFile.hpp"
#include "util.hpp"

extern uint8_t _FS_start;
extern uint8_t _FS_end;


size_t Filesystem::Filesystem::getFlashSize() {
    return reinterpret_cast<size_t>(&_FS_end) - reinterpret_cast<size_t>(&_FS_start);
}

Filesystem::Filesystem::Filesystem()
: ::FS(std::make_shared<Impl::Filesystem>(&_FS_start, &_FS_end - &_FS_start, FLASH_PAGE_SIZE, FLASH_SECTOR_SIZE, 16)) {
}

void Filesystem::Filesystem::begin() {
    if (!::FS::begin()) {
        ::FS::format();
        ::FS::begin();
    }
    if (!::FS::check()) {
        ::FS::format();
    }

    bool hasImgs = false;
    bool hasScenes = false;

    auto dir = openDir("/");
    while (dir.next()) {
        if (dir.isDirectory()) {
            if (dir.fileName().equals(IMAGES)) {
                hasImgs = true;
            } else if (dir.fileName().equals(SCENES)) {
                hasScenes = true;
            } else {
                rmdir(String("/") + dir.fileName());
            }
        } else {
            remove(String("/") + dir.fileName());
        }
    }

    if (!hasImgs) {
        mkdir(_IMAGES);
    }
    if (!hasScenes) {
        mkdir(_SCENES);
    }
}

Filesystem::File Filesystem::Filesystem::open(const String &path, const char *mode) {
    return open(path.c_str(), mode);
}

std::unique_ptr<Filesystem::File> Filesystem::Filesystem::openPtr(const String &path, const char *mode) {
    return openPtr(path.c_str(), mode);
}

Filesystem::File Filesystem::Filesystem::open(const char *path, const char *mode) {
    if (!_impl) {
        return ::Filesystem::File{};
    }

    OpenMode om;
    AccessMode am;
    if (!sflags(mode, om, am)) {
        return ::Filesystem::File{};
    }

    ::Filesystem::File f(_impl->open(path, om, am), this);
    f.setTimeCallback(_timeCallback);
    return f;
}

std::unique_ptr<Filesystem::File> Filesystem::Filesystem::openPtr(const char *path, const char *mode) {
    if (!_impl) {
        return std::make_unique<::Filesystem::File>();
    }

    OpenMode om;
    AccessMode am;
    if (!sflags(mode, om, am)) {
        return std::make_unique<::Filesystem::File>();
    }

    auto f = std::make_unique<::Filesystem::File>(_impl->open(path, om, am), this);
    f->setTimeCallback(_timeCallback);
    return f;
}

Filesystem::Dir Filesystem::Filesystem::openDir(const String& path) {
    return openDir(path.c_str());
}

std::unique_ptr<Filesystem::Dir> Filesystem::Filesystem::openDirPtr(const String &path) {
    return openDirPtr(path.c_str());
}

Filesystem::Dir Filesystem::Filesystem::openDir(const char* path) {
    if (!_impl) {
        return Dir{};
    }

    Dir d(_impl->openDir(path), this);
    d.setTimeCallback(_timeCallback);
    return d;
}

std::unique_ptr<Filesystem::Dir> Filesystem::Filesystem::openDirPtr(const char* path) {
    if (!_impl) {
        return std::make_unique<Dir>();
    }

    auto d = std::make_unique<Dir>(_impl->openDir(path), this);
    d->setTimeCallback(_timeCallback);
    return d;
}

Lib::SPI::Slave::Contract::FS::TmpFile * Filesystem::Filesystem::uploadFile(const char *path) {
    if (!_impl) {
        return new UploadFile();
    }

    OpenMode om;
    AccessMode am;
    if (!sflags("w", om, am)) {
        return new UploadFile();
    }

    auto tmp_name = (String(path) + ".tmp");
    remove(tmp_name);
    auto f = new UploadFile(_impl->open(tmp_name.c_str(), om, am), this, path);
    f->setTimeCallback(_timeCallback);
    return f;
}

lfs_t * Filesystem::Filesystem::getRawFS() {
    return reinterpret_cast<Impl::Filesystem*>(_impl.get())->getRawFS();
}
