//
// Created by Adam Makswiej on 03/12/2023.
//

#include "../../include/lib/spi/slave/buffer/FilesystemInputBuffer.hpp"


Lib::SPI::Slave::FilesystemInputBuffer::FilesystemInputBuffer(Contract::FS *filesystem, const char *basepath) {
    _filesystem = filesystem;
    _basepath = basepath;
}

bool Lib::SPI::Slave::FilesystemInputBuffer::open(const char *path, const BigSize size) {
    if (!_filesystem) {
        return false;
    }

    _flag.buf_a_ready = false;
    _flag.buf_b_ready = false;
    _size = size;
    _filename = strdup(path);
    return true;
}

bool Lib::SPI::Slave::FilesystemInputBuffer::loop() {
    if (_filename) {
        delete _file;

        _file = _filesystem->uploadFile((String(_basepath) + _filename).c_str());
        delete _filename; _filename = nullptr;

        return !!_file;
    }

    if (!_file) {
        return false;
    }

    if (_flag.buf_a_ready) {
        _flag.buf_a_ready = false;

        size_t size = sizeof(_buf_a);
        if (const size_t rest = _size - _file->position(); rest < size) {
            size = rest;
        }
        _file->write(const_cast<Data>(_buf_a), size);
        return true;
    }
    if (_flag.buf_b_ready) {
        _flag.buf_b_ready = false;

        size_t size = sizeof(_buf_b);
        if (const size_t rest = _size - _file->position(); rest < size) {
            size = rest;
        }
        _file->write(const_cast<Data>(_buf_b), size);
        return true;
    }

    return false;
}

void Lib::SPI::Slave::FilesystemInputBuffer::didChange() {
    _changed = true;
}

void Lib::SPI::Slave::FilesystemInputBuffer::trigger() {
    if (!_file) {
        return;
    }

    _file->flush();
    _file->completed();
    _SPIS_CB_IF_CALL(callback, context, _file);

    delete _file; _file = nullptr;
    _SPIS_CB_IF_CALL(callback_final, context_final);
}

bool Lib::SPI::Slave::FilesystemInputBuffer::validate() {
    if (!_file) {
        return false;
    }
    if (!_validator) {
        return true;
    }

    return _SPIS_CB_CALL(validator, context_validator, _file);
}

Lib::SPI::Slave::Buffer::BigSize Lib::SPI::Slave::FilesystemInputBuffer::size() const {
    return _size;
}

void Lib::SPI::Slave::FilesystemInputBuffer::set(const Offset offset, const Byte value) {
    const size_t buf_size = _flag.buf_b ? sizeof(_buf_b) : sizeof(_buf_a);
    const auto addr = offset % buf_size;

    if (_flag.buf_b) {
        _buf_b[addr] = value;
    } else {
        _buf_a[addr] = value;
    }

    if (addr + 1 == buf_size || offset + 1 >= _size) {
        if (_flag.buf_b) {
            _flag.buf_b_ready = true;
        } else {
            _flag.buf_a_ready = true;
        }
        _flag.buf_b = !_flag.buf_b;
    }
}

void Lib::SPI::Slave::FilesystemInputBuffer::reset() {
    if (!_file) {
        return;
    }
    _file->seek(0);
    _file->truncate(0);
}

void Lib::SPI::Slave::FilesystemInputBuffer::size(const BigSize size) {
    _size = size;
    if (_file && size < _file->size()) {
        _file->truncate(size);
    }
}

_SPIS_CB_CTX_DEF(Lib::SPI::Slave::FilesystemInputBuffer, callback_final, context_final, CallbackFinal)
_SPIS_CB_CTX_DEF(Lib::SPI::Slave::FilesystemInputBuffer, callback, context, Callback)
_SPIS_CB_CTX_DEF(Lib::SPI::Slave::FilesystemInputBuffer, validator, context_validator, Validator)

Lib::SPI::Slave::FilesystemInputBuffer::Progress Lib::SPI::Slave::FilesystemInputBuffer::progress() const {
    if (!_file) {
        return -1;
    }

    const size_t position = _file->position();
    if (position >= _size) {
        return 100;
    }

    return static_cast<Progress>(static_cast<uint16_t>(position) * 100 / _size);
}
