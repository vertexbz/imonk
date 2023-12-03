//
// Created by Adam Makswiej on 25/11/2023.
//

#include "./Interface.hpp"
#include <cstring>

using namespace Lib::SPI::Slave::CommandFactory;
auto buffer = "Testy test!";


Communication::Interface::Interface() {
    addCommand(VariableInputCommand(10, false)->grabBuffer(_in_buf));

    addCommand(VariableOutputCommand(20)->withBuffer<Lib::SPI::Slave::VariableOutputBuffer>([](auto &buf) {
        buf.update(reinterpret_cast<Lib::SPI::Slave::Buffer::Data>(const_cast<char*>(buffer)), strlen(buffer));
    }));

    addCommand(FilesystemInputCommand(30/*, filesystem*/));
}

void Communication::Interface::begin() {
    setup();
}
