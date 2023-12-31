//
// Created by Adam Makswiej on 02/12/2023.
//

#ifndef __SPISLAVELIB__COMMAND_FACTORY_HPP__
#define __SPISLAVELIB__COMMAND_FACTORY_HPP__

#include "./buffer/VariableInputBuffer.hpp"
#include "./buffer/TypedInputBuffer.hpp"
#include "./buffer/VariableOutputBuffer.hpp"
#include "./buffer/TypedOutputBuffer.hpp"
#include "./InputCommand.hpp"
#include "./OutputCommand.hpp"
#include "./FileUploadCommand.hpp"
#include "./NamedUploadCommand.hpp"

namespace Lib::SPI::Slave::CommandFactory {
#pragma mark Input - Variable
InputCommand *VariableInputCommand(
    Command::ID id,
    const VariableInputBuffer::Callback &callback = nullptr,
    bool deferred = false,
    const VariableInputBuffer::Validator &validator = nullptr,
    bool checksum = true
);

inline auto VariableInputCommand(const Command::ID id, const bool checksum) {
    return VariableInputCommand(id, nullptr, false, nullptr, checksum);
}

inline auto VariableInputCommand(const Command::ID id, const bool checksum, const VariableInputBuffer::Validator &validator) {
    return VariableInputCommand(id, nullptr, false, validator, checksum);
}

inline auto VariableInputCommand(const Command::ID id, const bool checksum, const VariableInputBuffer::Callback &callback, const bool deferred = false) {
    return VariableInputCommand(id, callback, deferred, nullptr, checksum);
}

inline auto VariableInputCommand(const Command::ID id, const VariableInputBuffer::Callback &callback, const bool deferred = false) {
    return VariableInputCommand(id, callback, deferred, nullptr, true);
}

#pragma mark Input - Typed
template<typename T>
InputCommand *TypedInputCommand(
    const Command::ID id,
    const typename TypedInputBuffer<T>::Callback &callback = nullptr,
    const bool deferred = false,
    const typename TypedInputBuffer<T>::Validator &validator = nullptr,
    const bool checksum = true
) {
    const auto buffer = new TypedInputBuffer<T>();
    buffer->callback(callback);
    buffer->validator(validator);
    return (new InputCommand(id, buffer))->checksum(checksum)->defer(deferred);
}

template<typename T>
auto TypedInputCommand(const Command::ID id, const bool checksum) {
    return TypedInputCommand<T>(id, nullptr, nullptr, false, nullptr, checksum);
}

template<typename T>
auto TypedInputCommand(const Command::ID id, const typename TypedInputBuffer<T>::Validator &validator, const bool checksum = true) {
    return TypedInputCommand<T>(id, nullptr, nullptr, false, validator, checksum);
}

template<typename T>
auto TypedInputCommand(const Command::ID id, const typename TypedInputBuffer<T>::Callback &callback, const bool deferred = false, const bool checksum = true) {
    return TypedInputCommand<T>(id, callback, nullptr, deferred, nullptr, checksum);
}

#pragma mark Input - Filesystem
inline FileUploadCommand *FileUploadCommand(const Command::ID id, Contract::FS *fs, const char *basepath = "/") {
    return new Slave::FileUploadCommand(id, fs, basepath);
}

inline NamedUploadCommand *NamedUploadCommand(const Command::ID id, Contract::FS *fs, const char *filepath) {
    return new Slave::NamedUploadCommand(id, fs, filepath);
}


#pragma mark Output - Variable
OutputCommand *VariableOutputCommand(
    Command::ID id,
    const VariableOutputBuffer::Callback &callback = nullptr,
    bool deferred = false
);

#pragma mark Output - Typed
template<typename T>
OutputCommand *TypedOutputCommand(
    const Command::ID id,
    const typename TypedOutputBuffer<T>::Callback &callback = nullptr,
    const bool deferred = false
) {
    const auto buffer = new TypedOutputBuffer<T>();
    buffer->callback(callback);
    return (new OutputCommand(id, buffer))->defer(deferred);
}

template<typename T>
inline auto TypedOutputCommand(const Command::ID id, const typename TypedOutputBuffer<T>::Callback &callback, const bool deferred) {
    return TypedOutputCommand<T>(id, callback, nullptr, deferred);
}
}

#endif // __SPISLAVELIB__COMMAND_FACTORY_HPP__
