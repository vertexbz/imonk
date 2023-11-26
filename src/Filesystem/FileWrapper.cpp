#include "FileWrapper.hpp"

Filesystem::FileWrapper::FileWrapper(File *file): DataWrapper(), _file(file) {

}

int Filesystem::FileWrapper::read(uint8_t *buf, uint32_t len) {
    return _file->readBytes(reinterpret_cast<char *>(buf), len);
}

void Filesystem::FileWrapper::skip(int32_t offset) {
    _file->seek(_file->position() + offset);
}

bool Filesystem::FileWrapper::seek(uint32_t offset) {
    return _file->seek(offset);
}

void Filesystem::FileWrapper::close() {

}

int32_t Filesystem::FileWrapper::tell() {
    return _file->position();
}
