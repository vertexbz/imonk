//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDOUTPUTBUFFER_HPP__
#define __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDOUTPUTBUFFER_HPP__

#include "./OutputBuffer.hpp"
#include "./TypedBuffer.hpp"


namespace Lib::SPI::Slave {
template<typename T>
class TypedOutputBuffer : public OutputBuffer, public TypedBuffer<T> {
protected:
    [[nodiscard]] Byte get(Offset offset) const override;

    void doneRedaing() override;

public:
    void update(const T &data);
};
}

#endif // __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDOUTPUTBUFFER_HPP__
