//
// Created by Adam Makswiej on 16/12/2023.
//

#ifndef __SPISLAVELIB__SPISLAVE_CONTRACT_FILESYSTEM_HPP__
#define __SPISLAVELIB__SPISLAVE_CONTRACT_FILESYSTEM_HPP__

#include <StreamString.h>

#include "../buffer/Buffer.hpp"
#if defined (ARDUINO_ARCH_MBED_RP2040) || defined(ARDUINO_ARCH_RP2040)
#include <hardware/flash.h>
#else
#define FLASH_PAGE_SIZE 256
#endif

namespace Lib::SPI::Slave::Contract {
class FS {
public:
    virtual ~FS() = default;

    class File {
    public:
        virtual ~File() = default;

        virtual Buffer::BigSize size() = 0;

        virtual Buffer::BigSize position() = 0;

        virtual bool seek(Buffer::Offset offset) = 0;

        virtual Buffer::BigSize write(Buffer::Data str, Buffer::BigSize size) = 0;

        virtual void flush() = 0;

        virtual void truncate(Buffer::BigSize size) = 0;
    };

    class TmpFile : public File {
    public:
        virtual void completed() = 0;
    };

    // TmpFile *uploadFile(const String& name) { return uploadFile(name.c_str()); }
    virtual TmpFile *uploadFile(const char * name) = 0;
};
}

#endif // __SPISLAVELIB__SPISLAVE_CONTRACT_FILESYSTEM_HPP__
