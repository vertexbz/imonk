//
// Created by Adam Makswiej on 16/12/2023.
//

#include "Filesystem.hpp"
#include "UploadFile.hpp"

Filesystem::UploadFile::UploadFile(FileImplPtr p, Filesystem *baseFS, const char *path): File(std::move(p), baseFS), _path(strdup(path)) {

}

Filesystem::UploadFile::~UploadFile() {
    File::flush();
    File::close();

    auto tmp_path = String(_path) + ".tmp";
    if (_completed && _path) {
        _baseFS->remove(_path);
        _baseFS->rename(tmp_path, _path);
    } else {
        _baseFS->remove(tmp_path);
    }
    _baseFS->gc();

    if (_got_crc) {
        lfs_setattr(((Filesystem *)_baseFS)->getRawFS(), _path, TagCRC, &_crc, sizeof(_crc));
        _got_crc = false;
    }

    delete _path;
}

void Filesystem::UploadFile::setCRC(Lib::CRC::CRC16 checksum) {
    _crc = checksum;
    _got_crc = true;
}

void Filesystem::UploadFile::completed() {
    _completed = true;
}

const char *Filesystem::UploadFile::path() const {
    return _path;
}

const char *Filesystem::UploadFile::basename() const {
    const char *p = _path;
    const char *slash = strrchr(p, '/');
    return (slash && slash[1]) ? slash + 1 : p;
}
