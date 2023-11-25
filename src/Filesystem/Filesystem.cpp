//
// Created by Adam Makswiej on 20/11/2023.
//

#include "Filesystem.hpp"
#include <LittleFS.h>

extern uint8_t _FS_start;
extern uint8_t _FS_end;


size_t Filesystem::Filesystem::getFlashSize() {
    return ((size_t)&_FS_end - (size_t)&_FS_start);
}

Filesystem::Filesystem::Filesystem(): FS(FSImplPtr(new littlefs_impl::LittleFSImpl(&_FS_start, &_FS_end - &_FS_start, 256, 4096, 16))) {

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

Dir Filesystem::Filesystem::images() {
    return openDir("/images");
}

Dir Filesystem::Filesystem::scenes() {
    return openDir("/scenes");
}
