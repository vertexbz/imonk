//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDBUFFER_HPP__
#define __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDBUFFER_HPP__

#include "./Buffer.hpp"
#include "../../../../../src/helper.callback.hpp"

namespace Lib::SPI::Slave {
template<typename T>
class TypedBuffer : virtual public Buffer {
protected:
    using Tref = std::conditional_t<std::is_scalar<T>{},
        std::remove_const_t<T>,
        std::add_lvalue_reference_t<std::add_volatile_t<T>>
    >;
public:
    _SPIS_CB_(void, Callback, Tref value);

protected:
    volatile T _data{0};
    Callback _callback = nullptr;
    _SPIS_CB_CTX(context);

    [[nodiscard]] BigSize size() const override;

public:
    _SPIS_CB_CTX_DECL(callback, context, Callback);

    void trigger() override;

    Tref value();
};
}

#endif // __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDBUFFER_HPP__
