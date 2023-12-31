//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__SPISLAVE_BUFFER_INPUTBUFFER_HPP__
#define __SPISLAVELIB__SPISLAVE_BUFFER_INPUTBUFFER_HPP__

#include "./Buffer.hpp"

namespace Lib::SPI::Slave {
class InputBuffer : virtual public Buffer {
    friend class InputCommand;

    volatile bool _changed = false;

protected:
    virtual void didChange();

    virtual void set(Offset offset, Byte value) = 0;

    virtual void reset() = 0;

    virtual bool validate() = 0;

public:
    bool changed();
};
}

#endif // __SPISLAVELIB__SPISLAVE_BUFFER_INPUTBUFFER_HPP__
