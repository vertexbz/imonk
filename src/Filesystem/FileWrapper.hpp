#pragma once

#include <lgfx/v1/misc/DataWrapper.hpp>
#include <FS.h>

namespace Filesystem {
struct FileWrapper : lgfx::DataWrapper {
protected:
    File *_file;
public:
    explicit FileWrapper(File *file);

    int read(uint8_t *buf, uint32_t len) override;

    void skip(int32_t offset) override;

    bool seek(uint32_t offset) override;

    void close() override;

    int32_t tell() override;

};
}
