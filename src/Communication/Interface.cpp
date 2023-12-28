//
// Created by Adam Makswiej on 25/11/2023.
//

#include "./Interface.hpp"

#include <Filesystem/UploadFile.hpp>

#include "./UpdateCommand.hpp"
#include "./CRCFileUploadCommand.hpp"
#include "./DirectoryManifest.hpp"
#include "./ViewStageCommand.hpp"
#include "./ViewSetVarCommand.hpp"
#include "../Filesystem/Filesystem.hpp"
#include "../StageManager.hpp"

using namespace Lib::SPI::Slave::CommandFactory;
using Data = Lib::SPI::Slave::Buffer::Data;
using Size = Lib::SPI::Slave::Buffer::Size;


struct Version {
    uint8_t major = 0;
    uint8_t minor = 0;
    uint8_t micro = 1;
};

Communication::Interface::Interface(Filesystem::Filesystem *fs, StageManager *stage_manager): _stage_manager(stage_manager), _filesystem(fs) {
    // Firmware
    addCommand(TypedOutputCommand<Version>(0x01));
    addCommand(_update = new UpdateCommand(0x0A, fs));
    addCommand(_reboot = new Lib::SPI::Slave::ExecCommand(0x0F, [] { rp2040.reboot(); }, true));

    // Imgaes
    addCommand(_remove_image = VariableInputCommand(0x10, true, [this](const Data data, Size _) {
        _filesystem->remove((String(Filesystem::Filesystem::_IMAGES_) + reinterpret_cast<char*>(data)).c_str());
    }, true));

    addCommand(_upload_image = new CRCFileUploadCommand(0x11, fs, Filesystem::Filesystem::_IMAGES_)); // todo validate .png
    addCommand(new DirectoryManifest(0x12, fs, Filesystem::Filesystem::_IMAGES));

    // Views
    addCommand(_remove_view = VariableInputCommand(0x20, true, [this](const Data data, Size _) {
        _filesystem->remove((String(Filesystem::Filesystem::_VIEWS_) + reinterpret_cast<char*>(data)).c_str());
        _stage_manager->forgetView(reinterpret_cast<char*>(data));
    }, true));

    addCommand(_upload_view = new CRCFileUploadCommand(0x21, fs, Filesystem::Filesystem::_VIEWS_));
    _upload_view->callback([this](const Filesystem::UploadFile *file) {
        _new_view = file->name(); // todo fix it, does not work
    });
    addCommand(new DirectoryManifest(0x22, fs, Filesystem::Filesystem::_VIEWS));

    // Action
    addCommand(_view_init = new ViewStageCommand(0x50, stage_manager));

    addCommand(_view_setvar = new ViewSetVarCommand(0x51, stage_manager));

    addCommand(_view_commit = TypedInputCommand<StageManager::StageId>(0x5E, [this](StageManager::StageId _) {
        _stage_manager->commit();
    }, true, [this](const StageManager::StageId &value) -> bool {
        return _stage_manager->stagedId() == value;
    }, false));

    addCommand(_view_abort = TypedInputCommand<StageManager::StageId>(0x5F, [this](StageManager::StageId _) {
        _stage_manager->abort();
    }, true, [this](const StageManager::StageId &value) -> bool {
        return _stage_manager->stagedId() == value;
    }, false));
}

void Communication::Interface::begin() {
    setup();
}

void Communication::Interface::loop() {
    _update->loop();
    _remove_image->loop();
    _upload_image->loop();
    _remove_view->loop();
    _upload_view->loop();
    _view_init->loop();
    _view_setvar->loop();
    _view_commit->loop();
    _view_abort->loop();
    _reboot->loop();
    if (_new_view != "") {
        _stage_manager->loadView(_new_view.c_str());
        _new_view = "";
    }
}
