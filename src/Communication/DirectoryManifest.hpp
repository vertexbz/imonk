#pragma once

#include "../Filesystem/Filesystem.hpp"
#include <vertaro/spi/slave.hpp>


namespace Communication {
class DirectoryManifest : public Lib::SPI::Slave::Command {
    Filesystem::Filesystem *_fs;
    const char *_path;
    std::unique_ptr<Filesystem::Dir> _dir = nullptr;
    String _name = "";
    Lib::CRC::CRC16 _crc;

    enum class ManifestEntryStage : uint8_t {
        CRCInit,
        CRC,
        Name,

        Unknown
    };

    volatile ManifestEntryStage _entry_stage = ManifestEntryStage::Unknown;
    volatile uint16_t _entry_stage_position = 0;
public:
    DirectoryManifest(ID id, Filesystem::Filesystem *fs, const char *basepath);

protected:
    Response handle(bool initial, Value value) override;
    bool expectedInput(Value value) override;

};
}
