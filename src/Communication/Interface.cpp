//
// Created by Adam Makswiej on 25/11/2023.
//

#include "./Interface.hpp"
#include <cstring>

using Lib::SPI::Slave::CommandFactory;
auto buffer = "Testy test!";


Communication::Interface::Interface() {
    addCommand(CommandFactory::input_variable(20, false)->grabBuffer(_in_buf));

    addCommand(CommandFactory::output_variable(21)->withBuffer<Lib::SPI::Slave::VariableOutputBuffer>([](auto &buf) {
        buf.update(reinterpret_cast<Lib::SPI::Slave::Buffer::Data>(const_cast<char*>(buffer)), strlen(buffer));
    }));
}

void Communication::Interface::begin() {
    setup();
}
