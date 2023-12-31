//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__SPISLAVE_BUFFER_OUTPUTBUFFER_HPP__
#define __SPISLAVELIB__SPISLAVE_BUFFER_OUTPUTBUFFER_HPP__

#include "./Buffer.hpp"

namespace Lib::SPI::Slave {
class OutputBuffer : virtual public Buffer {
    friend class OutputCommand;

    volatile bool _read = false;

protected:
    virtual void doneRedaing();;

    [[nodiscard]] virtual Byte get(Offset offset) const = 0;

public:
    bool read();
};
}

#endif // __SPISLAVELIB__SPISLAVE_BUFFER_OUTPUTBUFFER_HPP__
