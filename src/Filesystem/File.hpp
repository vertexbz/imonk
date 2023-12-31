#pragma once

#include <lib/spi/slave/contract/filesytem.hpp>
#include <lgfx/v1/misc/DataWrapper.hpp>
#include <lib/crc.hpp>
#include "Impl/File.hpp"

namespace Filesystem {
class File : public fs::File, public lgfx::DataWrapper, public Lib::SPI::Slave::Contract::FS::TmpFile {
public:
    using Tag = Impl::File::Tag;
    static constexpr Tag TagCRC = 'C';

    using fs::File::File;

    int read(uint8_t *buf, uint32_t len) override;

    void skip(int32_t offset) override;

    bool seek(uint32_t offset) override;

    void close() override;

    int32_t tell() override;

    Lib::SPI::Slave::Buffer::BigSize size() override;

    Lib::SPI::Slave::Buffer::BigSize position() override;

    void flush() override;

    void truncate(Lib::SPI::Slave::Buffer::BigSize size) override;

    Lib::SPI::Slave::Buffer::BigSize write(Lib::SPI::Slave::Buffer::Data str, Lib::SPI::Slave::Buffer::BigSize size) override;

    virtual void setCRC(Lib::CRC::CRC16 checksum);

    Lib::CRC::CRC16 getCRC();

    void completed() override;
};
}
