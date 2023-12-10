#pragma once

#include <vertaro/spi/slave.impl.hpp>

namespace Filesystem {
class Filesystem;
}

namespace Communication {
class UpdateCommand;

class Interface final : protected Lib::SPI::Slave::Interface {
    Lib::SPI::Slave::FileUploadCommand *_upload_image = nullptr;
    Lib::SPI::Slave::FileUploadCommand *_upload_scene = nullptr;
    UpdateCommand *_update = nullptr;
protected:
    Lib::SPI::Slave::VariableInputBuffer *_in_buf = nullptr;

public:
    explicit Interface(Filesystem::Filesystem *fs);

    void begin();

    void loop();

    [[nodiscard]] Lib::SPI::Slave::VariableInputBuffer *inBuf() const {
        return _in_buf;
    }

    [[nodiscard]] Lib::SPI::Slave::FileUploadCommand *upImg() const {
        return _upload_image;
    }
};
}
