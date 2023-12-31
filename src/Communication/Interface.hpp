#pragma once

#include <lib/spi/slave.impl.hpp>

class StageManager;

namespace Filesystem {
class Filesystem;
}

namespace Communication {
class UpdateCommand;
class ViewStageCommand;
class ViewSetVarCommand;
class CRCFileUploadCommand;

class Interface final : protected Lib::SPI::Slave::Interface {
    StageManager *_stage_manager;
    Filesystem::Filesystem *_filesystem;
    Lib::SPI::Slave::ExecCommand *_reboot = nullptr;
    Lib::SPI::Slave::InputCommand *_remove_image = nullptr;
    Lib::SPI::Slave::InputCommand *_remove_view = nullptr;
    CRCFileUploadCommand *_upload_image = nullptr;
    CRCFileUploadCommand *_upload_view = nullptr;
    Lib::SPI::Slave::InputCommand *_view_commit = nullptr;
    Lib::SPI::Slave::InputCommand *_view_abort = nullptr;
    ViewStageCommand *_view_init = nullptr;
    ViewSetVarCommand *_view_setvar = nullptr;
    UpdateCommand *_update = nullptr;
    String _new_view = "";
public:
    explicit Interface(Filesystem::Filesystem *fs, StageManager *sm);

    void begin();

    void loop();
};
}
