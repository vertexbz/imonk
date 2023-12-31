//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDBUFFER_IMPL_HPP__
#define __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDBUFFER_IMPL_HPP__

#include "./TypedBuffer.hpp"

template<typename T>
Lib::SPI::Slave::Buffer::BigSize Lib::SPI::Slave::TypedBuffer<T>::size() const {
    return sizeof(T);
}

_SPIS_CB_CTX_DEF_TMPL(typename T, Lib::SPI::Slave::TypedBuffer<T>, callback, context, Callback)

template<typename T>
void Lib::SPI::Slave::TypedBuffer<T>::trigger() {
    _SPIS_CB_IF_CALL(callback, context, _data);
}


template<typename T>
typename Lib::SPI::Slave::TypedBuffer<T>::Tref Lib::SPI::Slave::TypedBuffer<T>::value() {
    return this->_data;
}

#endif // __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDBUFFER_IMPL_HPP__
