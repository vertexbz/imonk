//
// Created by Adam Makswiej on 20/11/2023.
//

#include <LittleFS.h>
#include <hardware/flash.h>
#include "Filesystem.hpp"
#include "util.hpp"

extern uint8_t _FS_start;
extern uint8_t _FS_end;


size_t Filesystem::Filesystem::getFlashSize() {
    return reinterpret_cast<size_t>(&_FS_end) - reinterpret_cast<size_t>(&_FS_start);
}

Filesystem::Filesystem::Filesystem()
: FS(std::make_shared<littlefs_impl::LittleFSImpl>(&_FS_start, &_FS_end - &_FS_start, FLASH_PAGE_SIZE, FLASH_SECTOR_SIZE, 16)) {
}

void Filesystem::Filesystem::begin() {
    if (!FS::begin()) {
        FS::format();
        FS::begin();
    }
    if (!FS::check()) {
        FS::format();
    }

    bool hasImgs = false;
    bool hasScenes = false;

    auto dir = openDir("/");
    while (dir.next()) {
        if (dir.isDirectory()) {
            if (dir.fileName().equals("images")) {
                hasImgs = true;
            } else if (dir.fileName().equals("scenes")) {
                hasScenes = true;
            } else {
                rmdir(String("/") + dir.fileName());
            }
        } else {
            remove(String("/") + dir.fileName());
        }
    }

    if (!hasImgs) {
        mkdir("/images");
    }
    if (!hasScenes) {
        mkdir("/scenes");
    }
}

Filesystem::Dir Filesystem::Filesystem::images() {
    return openDir("/images");
}

Filesystem::Dir Filesystem::Filesystem::scenes() {
    return openDir("/scenes");
}

Filesystem::File Filesystem::Filesystem::open(const String &path, const char *mode) {
    return open(path.c_str(), mode);
}

Filesystem::File Filesystem::Filesystem::open(const char *path, const char *mode) {
    if (!_impl) {
        return File{};
    }

    fs::OpenMode om;
    fs::AccessMode am;
    if (!sflags(mode, om, am)) {
        return File{};
    }

    File f(_impl->open(path, om, am), this);
    f.setTimeCallback(_timeCallback);
    return f;
}

Filesystem::Dir Filesystem::Filesystem::openDir(const String& path) {
    return openDir(path.c_str());
}

Filesystem::Dir Filesystem::Filesystem::openDir(const char* path) {
    if (!_impl) {
        return Dir{};
    }

    Dir d(_impl->openDir(path), this);
    d.setTimeCallback(_timeCallback);
    return d;
}
