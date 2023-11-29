//
// Created by Adam Makswiej on 25/11/2023.
//

#include "./Interface.hpp"

InterfaceData outputBuffer;


Communication::Interface::Interface() {
    addCommand(new Lib::SPI::Slave::OutputCommand<InterfaceData>(1));
    addCommand(new Lib::SPI::Slave::OutputCommand<uint16_t>(2));
    addCommand(new Lib::SPI::Slave::InputCommand<uint32_t>(10, 5));
    addCommand(new Lib::SPI::Slave::InputCommand<InterfaceData>(11));
    addCommand(new Lib::SPI::Slave::InputCommand<uint32_t>(12, 64));
    addCommand(new Lib::SPI::Slave::InputCommand<InterfaceData>(13, outputBuffer));
}

void Communication::Interface::begin() {
    setup();
    outputBuffer.chr = 7;
    outputBuffer.word = 8765;
    outputBuffer.tf = true;
    output()->update(&outputBuffer);

    output2()->update(1111);
}
