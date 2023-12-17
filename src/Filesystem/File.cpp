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
    return fs::File::position();
}

Lib::SPI::Slave::Buffer::BigSize Filesystem::File::size() {
    return fs::File::size();
}

Lib::SPI::Slave::Buffer::BigSize Filesystem::File::position() {
    return fs::File::position();
}

void Filesystem::File::flush() {
    fs::File::flush();
}

void Filesystem::File::truncate(const Lib::SPI::Slave::Buffer::BigSize size) {
    fs::File::truncate(size);
}

Lib::SPI::Slave::Buffer::BigSize Filesystem::File::write(Lib::SPI::Slave::Buffer::Data str, Lib::SPI::Slave::Buffer::BigSize size) {
    return fs::File::write(str, size);
}

void Filesystem::File::setCRC(const Lib::CRC::CRC16 checksum) {
    reinterpret_cast<Impl::File*>(_p.get())->setTag(TagCRC, &checksum, sizeof(checksum));
}

Lib::CRC::CRC16 Filesystem::File::getCRC() {
    Lib::CRC::CRC16 checksum;
    if (reinterpret_cast<Impl::File*>(_p.get())->getTag(TagCRC, &checksum, sizeof(checksum))) {
        return checksum;
    }

    return Lib::CRC::Initial16;
}

void Filesystem::File::completed() {
}
