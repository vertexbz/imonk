//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDOUTPUTBUFFER_IMPL_HPP__
#define __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDOUTPUTBUFFER_IMPL_HPP__

#include "./TypedOutputBuffer.hpp"
#include "./TypedBuffer.impl.hpp"

template<typename T>
Lib::SPI::Slave::Buffer::Byte Lib::SPI::Slave::TypedOutputBuffer<T>::get(const Offset offset) const {
    if (offset < sizeof(T)) {
        return reinterpret_cast<Data>(const_cast<T *>(&this->_data))[offset];
    }
    return Command::BadCall;
}

template<typename T>
void Lib::SPI::Slave::TypedOutputBuffer<T>::doneRedaing() {
    OutputBuffer::doneRedaing();
    // todo ensure integrity and double bufferring
}

template<typename T>
void Lib::SPI::Slave::TypedOutputBuffer<T>::update(const T &data) {
    memcpy((void*)&this->_data, &data, sizeof(T));
}

#endif // __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDOUTPUTBUFFER_IMPL_HPP__
