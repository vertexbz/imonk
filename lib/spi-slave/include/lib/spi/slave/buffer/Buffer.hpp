//
// Created by Adam Makswiej on 2019-07-12.
//

#ifndef __SPISLAVELIB__SPISLAVE_BUFFER_HPP__
#define __SPISLAVELIB__SPISLAVE_BUFFER_HPP__

#include <cstdint>


namespace Lib::SPI::Slave {
class InputCommand;
class OutputCommand;

class Buffer {
    friend class InputCommand;
    friend class OutputCommand;

public:
    typedef uint32_t BigSize;
    typedef uint16_t Size;
    typedef uint8_t Byte;

    typedef Byte *Data;
    typedef uint32_t Offset;

protected:
    [[nodiscard]] virtual BigSize size() const = 0;

    virtual void trigger() = 0;

public:
    virtual ~Buffer() = default;
};


}
#endif
