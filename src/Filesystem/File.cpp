//
// Created by Adam Makswiej on 09/12/2023.
//

#include "File.hpp"

int Filesystem::File::read(uint8_t *buf, const uint32_t len) {
    return readBytes(reinterpret_cast<char *>(buf), len);
}

void Filesystem::File::skip(const int32_t offset) {
    fs::File::seek(position() + offset);
}

bool Filesystem::File::seek(const uint32_t offset) {
    return fs::File::seek(offset);
}

void Filesystem::File::close() {
    fs::File::close();
}

int32_t Filesystem::File::tell() {
    return position();
}
