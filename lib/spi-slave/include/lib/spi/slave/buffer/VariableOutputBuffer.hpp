//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__SPISLAVE_BUFFER_VARIABLEOUTPUTBUFFER_HPP__
#define __SPISLAVELIB__SPISLAVE_BUFFER_VARIABLEOUTPUTBUFFER_HPP__

#include "./OutputBuffer.hpp"
#include "./VariableBuffer.hpp"

namespace Lib::SPI::Slave {
class VariableOutputBuffer : public OutputBuffer, public VariableBuffer {
protected:
    [[nodiscard]] Byte get(Offset offset) const override;

    void doneRedaing() override;

public:
    void update(Data data, Size size);
};
}

#endif // __SPISLAVELIB__SPISLAVE_BUFFER_VARIABLEOUTPUTBUFFER_HPP__
