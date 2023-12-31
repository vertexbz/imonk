//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDINPUTBUFFER_IMPL_HPP__
#define __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDINPUTBUFFER_IMPL_HPP__

#include "./TypedInputBuffer.hpp"
#include "./TypedBuffer.impl.hpp"

template<typename T>
void Lib::SPI::Slave::TypedInputBuffer<T>::set(const Offset offset, const Byte value) {
    if (offset < sizeof(T)) {
        reinterpret_cast<Data>(const_cast<T *>(&this->_data))[offset] = value;
    }
}

template<typename T>
bool Lib::SPI::Slave::TypedInputBuffer<T>::validate() {
    if (!_validator) {
        return true;
    }

    return _SPIS_CB_CALL(validator, context_validator, this->_data);
}

template<typename T>
void Lib::SPI::Slave::TypedInputBuffer<T>::reset() {

}

_SPIS_CB_CTX_DEF_TMPL(typename T, Lib::SPI::Slave::TypedInputBuffer<T>, validator, context_validator, Validator)


#endif // __SPISLAVELIB__SPISLAVE_BUFFER_TYPEDINPUTBUFFER_IMPL_HPP__
