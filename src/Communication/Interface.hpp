#pragma once

#include <vertaro/spi/slave.impl.hpp>

namespace Filesystem {
class Filesystem;
}

namespace Communication {
class UpdateCommand;

class Interface final : protected Lib::SPI::Slave::Interface {
    Lib::SPI::Slave::ExecCommand *_reboot = nullptr;
    Lib::SPI::Slave::InputCommand *_remove_image = nullptr;
    Lib::SPI::Slave::InputCommand *_remove_scene = nullptr;
    Lib::SPI::Slave::FileUploadCommand *_upload_image = nullptr;
    Lib::SPI::Slave::FileUploadCommand *_upload_scene = nullptr;
    UpdateCommand *_update = nullptr;
public:
    explicit Interface(Filesystem::Filesystem *fs);

    void begin();

    void loop();

    [[nodiscard]] Lib::SPI::Slave::FileUploadCommand *upImg() const { // todo
        return _upload_image;
    }
};
}
