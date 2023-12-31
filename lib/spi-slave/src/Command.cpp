//
// Created by Adam Makswiej on 22/05/2022.
//

#include "../include/lib/spi/slave/Command.hpp"

Lib::SPI::Slave::Command::Command(ID id) {
    _id = id;
}

Lib::SPI::Slave::Command::ID Lib::SPI::Slave::Command::id() const {
    return _id;
}

void Lib::SPI::Slave::Command::setup() {}

bool Lib::SPI::Slave::Command::expectedInput(Value value) {
    return true;
}

void Lib::SPI::Slave::Command::loop() {

}

Lib::SPI::Slave::Command::Response Lib::SPI::Slave::Command::passTo(Command *command, const bool initial, const Value value) {
    return command->handle(initial, value);
}
