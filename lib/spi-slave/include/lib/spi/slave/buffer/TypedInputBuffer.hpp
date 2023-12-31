//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDINPUTBUFFER_HPP__
#define __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDINPUTBUFFER_HPP__

#include "./InputBuffer.hpp"
#include "./TypedBuffer.hpp"
#include <functional>

namespace Lib::SPI::Slave {
template<typename T>
class TypedInputBuffer : public InputBuffer, public TypedBuffer<T> {
protected:
    using Tref = std::conditional_t<std::is_scalar<T>{},
        std::remove_const_t<T>,
        std::add_lvalue_reference_t<std::add_volatile_t<T>>
    >;
public:
    _SPIS_CB_(bool, Validator, Tref value);

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

#endif // __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDINPUTBUFFER_HPP__
