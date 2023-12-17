#pragma once

#include <vertaro/spi/slave/contract/filesytem.hpp>
#include "Dir.hpp"
#include "File.hpp"

namespace Filesystem {
class Filesystem : public FS, public Lib::SPI::Slave::Contract::FS {
public:
    static constexpr auto *_IMAGES_ = "/images/";
    static constexpr auto *_IMAGES = "/images";
    static constexpr auto *IMAGES = "images";
    static constexpr auto *_SCENES_ = "/scenes/";
    static constexpr auto *_SCENES = "/scenes";
    static constexpr auto* SCENES = "scenes";

    static size_t getFlashSize();

    Filesystem();

    void begin();

    Dir openDir(const String& path);

    Dir openDir(const char* path);

    ::Filesystem::File open(const String& path, const char* mode);

    ::Filesystem::File open(const char* path, const char* mode);

    std::unique_ptr<Dir> openDirPtr(const String& path);

    std::unique_ptr<Dir> openDirPtr(const char* path);

    std::unique_ptr<::Filesystem::File> openPtr(const String& path, const char* mode);

    std::unique_ptr<::Filesystem::File> openPtr(const char* path, const char* mode);

    TmpFile * uploadFile(const char *name) override;

    lfs_t *getRawFS();
};
}
