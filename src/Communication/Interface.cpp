//
// Created by Adam Makswiej on 25/11/2023.
//

#include "./Interface.hpp"
#include <cstring>

auto buffer = "Testy test!";


Communication::Interface::Interface() {
    _in_buf = new Lib::SPI::Slave::VariableInputBuffer();
    addCommand((new Lib::SPI::Slave::VarInputCommand(20, _in_buf))->checksum(false));

    _out_buf = new Lib::SPI::Slave::VariableOutputBuffer();
    addCommand(new Lib::SPI::Slave::VarOutputCommand(21, _out_buf));
}

void Communication::Interface::begin() {
    _out_buf->update(reinterpret_cast<Lib::SPI::Slave::Buffer::Data>(const_cast<char*>(buffer)), strlen(buffer));

    setup();
}
