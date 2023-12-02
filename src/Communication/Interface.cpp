//
// Created by Adam Makswiej on 25/11/2023.
//

#include "./Interface.hpp"
#include <cstring>

InterfaceData outputBuffer;
auto buffer = "Testy test!";


Communication::Interface::Interface() {
    addCommand(new Lib::SPI::Slave::OutputCommand<InterfaceData>(1));
    addCommand(new Lib::SPI::Slave::OutputCommand<uint16_t>(2));
    addCommand(new Lib::SPI::Slave::InputCommand<uint32_t>(10, 5));
    addCommand(new Lib::SPI::Slave::InputCommand<InterfaceData>(11));
    addCommand(new Lib::SPI::Slave::InputCommand<uint32_t>(12, 64));
    addCommand(new Lib::SPI::Slave::InputCommand<InterfaceData>(13, outputBuffer));

    _in_buf = new Lib::SPI::Slave::VariableInputBuffer();
    addCommand((new Lib::SPI::Slave::VarInputCommand(20, _in_buf))->checksum(false));

    _out_buf = new Lib::SPI::Slave::VariableOutputBuffer();
    addCommand(new Lib::SPI::Slave::VarOutputCommand(21, _out_buf));
}

void Communication::Interface::begin() {
    _out_buf->update(reinterpret_cast<Lib::SPI::Slave::Buffer::Data>(const_cast<char*>(buffer)), strlen(buffer));

    outputBuffer.chr = 7;
    outputBuffer.word = 8765;
    outputBuffer.tf = true;
    output()->update(&outputBuffer);

    output2()->update(1111);

    setup();
}
