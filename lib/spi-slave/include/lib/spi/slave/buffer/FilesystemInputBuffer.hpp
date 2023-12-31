//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__FILESYSTEMINPUTBUFFER_HPP__
#define __SPISLAVELIB__FILESYSTEMINPUTBUFFER_HPP__

#include "../contract/filesytem.hpp"
#include "./InputBuffer.hpp"
#include "../../../../../src/helper.callback.hpp"

#include <lib/crc.hpp>

namespace Lib::SPI::Slave {
class FileUploadCommand;
class UploadCommand;

class FilesystemInputBuffer : public InputBuffer {
    friend class FileUploadCommand;
    friend class UploadCommand;
public:
    _SPIS_CB_(bool, Validator, Contract::FS::File *file);
    _SPIS_CB_(void, Callback, Contract::FS::File *file);
    _SPIS_CB_(void, CallbackFinal);
    typedef int8_t Progress;
protected:
    volatile Byte _buf_a[FLASH_PAGE_SIZE]{};
    volatile Byte _buf_b[FLASH_PAGE_SIZE]{};

    Contract::FS *_filesystem = nullptr;
    Contract::FS::TmpFile *_file = nullptr;
    BigSize _size = 0;
    const char *_filename = nullptr;
    const char *_basepath;

    volatile bool _changed = false;

    Callback _callback = nullptr;
    _SPIS_CB_CTX(context);
    CallbackFinal _callback_final = nullptr;
    _SPIS_CB_CTX(context_final);
    Validator _validator = nullptr;
    _SPIS_CB_CTX(context_validator);

    volatile struct {
        volatile bool buf_b = false;
        volatile bool buf_a_ready = false;
        volatile bool buf_b_ready = false;
    } _flag{};

    void didChange() override;

    void trigger() override;

    bool validate() override;

    [[nodiscard]] BigSize size() const override;

    void set(Offset offset, Byte value) override;

    void reset() override;
public:
    explicit FilesystemInputBuffer(Contract::FS *filesystem, const char *basepath = "/");

    bool open(const char *path, BigSize size);

    bool loop();

    void size(BigSize size);

    _SPIS_CB_CTX_DECL(callback_final, context_final, CallbackFinal);
    _SPIS_CB_CTX_DECL(callback, context, Callback);
    _SPIS_CB_CTX_DECL(validator, context_validator, Validator);

    [[nodiscard]] Progress progress() const;
};
}

#endif // __SPISLAVELIB__FILESYSTEMINPUTBUFFER_HPP__
