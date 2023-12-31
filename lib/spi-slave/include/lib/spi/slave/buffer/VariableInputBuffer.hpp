//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__SPISLAVE_BUFFER_VARIABLEINPUTBUFFER_HPP__
#define __SPISLAVELIB__SPISLAVE_BUFFER_VARIABLEINPUTBUFFER_HPP__

#include "./InputBuffer.hpp"
#include "./VariableBuffer.hpp"
#include <functional>

namespace Lib::SPI::Slave {
class VariableInputBuffer : public InputBuffer, public VariableBuffer {
public:
    _SPIS_CB_(bool, Validator, Data data, Size size);

protected:
    Validator _validator = nullptr;
    _SPIS_CB_CTX(context_validator);

    void set(Offset offset, Byte value) override;

    bool validate() override;

    void reset() override;

public:
    _SPIS_CB_CTX_DECL(validator, context_validator, Validator);
};
}

#endif // __SPISLAVELIB__SPISLAVE_BUFFER_VARIABLEINPUTBUFFER_HPP__
