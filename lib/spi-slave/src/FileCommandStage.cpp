//
// Created by Adam Makswiej on 03/12/2023.
//

#include "../include/lib/spi/slave/FileCommandStage.hpp"
#include <type_traits>


Lib::SPI::Slave::FileCommandStage & Lib::SPI::Slave::operator++(FileCommandStage &s) {
    if (s == FileCommandStage::Unknown) {
        return s;
    }

    s = static_cast<FileCommandStage>(static_cast<std::underlying_type_t<FileCommandStage>>(s) + 1);

    return s;
}

const Lib::SPI::Slave::FileCommandStage Lib::SPI::Slave::operator++(FileCommandStage &s, int) {
    const FileCommandStage result = s;
    ++s;
    return result;
}
