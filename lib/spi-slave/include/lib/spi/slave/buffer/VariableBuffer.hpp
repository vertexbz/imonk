//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__SPISLAVE_BUFFER_VARIABLEBUFFER_HPP__
#define __SPISLAVELIB__SPISLAVE_BUFFER_VARIABLEBUFFER_HPP__

#include "./Buffer.hpp"
#include "../../../../../src/helper.callback.hpp"

namespace Lib::SPI::Slave {
class VariableBuffer : virtual public Buffer {
public:
    _SPIS_CB_(void, Callback, Data data, Size size);

protected:
    volatile Size _size = 0;
    Data _data = nullptr;

    Callback _callback = nullptr;
    _SPIS_CB_CTX(context);

    [[nodiscard]] BigSize size() const override;

    void trigger() override;

public:
    ~VariableBuffer() override;

    _SPIS_CB_CTX_DECL(callback, context, Callback);

    [[nodiscard]] const Data value() const;
    [[nodiscard]] const Size length() const;
};
}

#endif // __SPISLAVELIB__SPISLAVE_BUFFER_VARIABLEBUFFER_HPP__
