//
// Created by Adam Makswiej on 25/11/2023.
//

#include "./Interface.hpp"
#include <cstring>
#include <Filesystem/Filesystem.hpp>

using namespace Lib::SPI::Slave::CommandFactory;
auto buffer = "Testy test!";

struct Version {
    uint8_t major = 0;
    uint8_t minor = 0;
    uint8_t micro = 1;
} ;

Communication::Interface::Interface(Filesystem::Filesystem *fs) {
    addCommand(TypedOutputCommand<Version>(0x01));
    // 0x0F update

    // 0x10 get images manifest
    // 0x11 upload image
    addCommand(_upload_image = FilesystemInputCommand(0x11, fs, "/images/")); // todo validate .png

    // 0x20 get scenes manifest
    // 0x21 upload scene
    addCommand(_upload_scene = FilesystemInputCommand(0x21, fs, "/scenes/")); // todo validate .json

    // 0x30 set scene
    // 0x31 set variable int vs float vs str!?


    addCommand(VariableInputCommand(0xA0, false)->grabBuffer(_in_buf));
    addCommand(VariableOutputCommand(0xA1)->withBuffer<Lib::SPI::Slave::VariableOutputBuffer>([](auto &buf) {
        buf.update(reinterpret_cast<Lib::SPI::Slave::Buffer::Data>(const_cast<char*>(buffer)), strlen(buffer));
    }));
    addCommand(TypedInputCommand<Version>(0xA2)->checksum(false));
}

void Communication::Interface::begin() {
    setup();
}

void Communication::Interface::loop() {
    _upload_image->loop();
    _upload_scene->loop();
}
