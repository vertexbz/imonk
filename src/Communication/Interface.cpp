//
// Created by Adam Makswiej on 25/11/2023.
//

#include "./Interface.hpp"
#include "./UpdateCommand.hpp"
#include "./CRCFileUploadCommand.hpp"
#include "./DirectoryManifest.hpp"
#include "../Filesystem/Filesystem.hpp"

using namespace Lib::SPI::Slave::CommandFactory;
using Data = Lib::SPI::Slave::Buffer::Data;
using Size = Lib::SPI::Slave::Buffer::Size;


struct Version {
    uint8_t major = 0;
    uint8_t minor = 0;
    uint8_t micro = 1;
};

Communication::Interface::Interface(Filesystem::Filesystem *fs) {
    // Firmware
    addCommand(TypedOutputCommand<Version>(0x01));
    addCommand(_update = new UpdateCommand(0x0A, fs));
    addCommand(_reboot = new Lib::SPI::Slave::ExecCommand(0x0F, [] { rp2040.reboot(); }, true));

    // Imgaes
    addCommand(_remove_image = VariableInputCommand(0x10, true, [fs](const Data data, Size _) {
        fs->remove((String(Filesystem::Filesystem::_IMAGES_) + reinterpret_cast<char*>(data)).c_str());
    }, true));

    addCommand(_upload_image = new CRCFileUploadCommand(0x11, fs, Filesystem::Filesystem::_IMAGES_)); // todo validate .png
    addCommand(new DirectoryManifest(0x12, fs, Filesystem::Filesystem::_IMAGES));

    // Scenes
    addCommand(_remove_scene = VariableInputCommand(0x20, true, [fs](const Data data, Size _) {
        fs->remove((String(Filesystem::Filesystem::_SCENES_) + reinterpret_cast<char*>(data)).c_str());
    }, true));

    addCommand(_upload_scene = new CRCFileUploadCommand(0x21, fs, Filesystem::Filesystem::_SCENES_)); // todo validate .json
    addCommand(new DirectoryManifest(0x22, fs, Filesystem::Filesystem::_SCENES));

    // Action
}

void Communication::Interface::begin() {
    setup();
}

void Communication::Interface::loop() {
    _update->loop();
    _remove_image->loop();
    _upload_image->loop();
    _remove_scene->loop();
    _upload_scene->loop();
    _reboot->loop();
}
