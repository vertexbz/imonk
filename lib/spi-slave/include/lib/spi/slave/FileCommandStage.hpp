//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__FILECOMMANDSTAGE_HPP__
#define __SPISLAVELIB__FILECOMMANDSTAGE_HPP__

#include <cstdint>

namespace Lib::SPI::Slave {
enum class FileCommandStage : uint8_t {
    Name,
    InitSize,
    Size,
    InitData,
    Data,

    Unknown
};

FileCommandStage & operator++(FileCommandStage &s);

const FileCommandStage operator++(FileCommandStage &s, int);

}

#endif // __SPISLAVELIB__FILECOMMANDSTAGE_HPP__
