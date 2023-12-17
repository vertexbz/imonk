//
// Created by Adam Makswiej on 16/12/2023.
//

#include "DirectoryManifest.hpp"

Communication::DirectoryManifest::DirectoryManifest(const ID id, Filesystem::Filesystem *fs, const char *basepath)
: Command(id), _fs(fs), _path(basepath) {
}

Lib::SPI::Slave::Command::Response Communication::DirectoryManifest::handle(bool initial, Value value) {
    if (initial) {
        _entry_stage = ManifestEntryStage::CRC;
        _entry_stage_position = 0;
        _dir = _fs->openDirPtr(_path);
        if (!_dir->next()) {
            return Response{.value = Empty, .next = false};
        }
        _dir->firstFile();

        _crc = _dir->fileCRC();

        return Response{.value = GoOn, .next = true};
    }

    if (!_dir || value != 0) {
        _dir = nullptr;
        _name = "";
        return Response{.value = BadCall, .next = false};
    }

    if (_entry_stage == ManifestEntryStage::CRCInit) {
        _crc = _dir->fileCRC();
        _entry_stage = ManifestEntryStage::CRC;
        return Response{.value = GoOn, .next = true};
    }

    if (_entry_stage == ManifestEntryStage::CRC) {
        const auto resp = reinterpret_cast<Lib::SPI::Slave::Buffer::Byte *>(&_crc)[_entry_stage_position++];
        if (_entry_stage_position == sizeof(_crc)) {
            _entry_stage_position = 0;
            _entry_stage = ManifestEntryStage::Name;
            _name = _dir->fileName();
        }
        return Response{.value = resp, .next = true};
    }

    if (_entry_stage == ManifestEntryStage::Name) {
        const auto resp = static_cast<Lib::SPI::Slave::Buffer::Byte>(_name.charAt(_entry_stage_position++));
        if (resp == '\0') {
            _entry_stage_position = 0;
            if (_dir->nextFile()) {
                _entry_stage = ManifestEntryStage::CRCInit;
            } else {
                _dir = nullptr;
                _name = "";
                _entry_stage = ManifestEntryStage::Unknown;
            }
        }

        return Response{.value = resp, .next = true};
    }


    return Response{.value = BadCall, .next = false};
}

bool Communication::DirectoryManifest::expectedInput(Value value) {
    return _entry_stage != ManifestEntryStage::Unknown;
}
