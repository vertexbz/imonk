//
// Created by Adam Makswiej on 2019-07-12.
//

#ifndef __SPISLAVELIB__SPISLAVE_COMMAND_INPUT_HPP__
#define __SPISLAVELIB__SPISLAVE_COMMAND_INPUT_HPP__

#include "Command.hpp"
#include "./buffer/InputBuffer.hpp"
#include <lib/crc.hpp>
#include <type_traits>
#include <functional>


namespace Lib::SPI::Slave {
class FileUploadCommand;
class UploadCommand;

class InputCommand : public Command {
    volatile Buffer::Offset _position = 0;
    volatile Buffer::Offset _top_position = 0;
protected:
    volatile CRC::CRC16 _crc_computed = 0;
    volatile CRC::CRC16 _crc = 0;
    InputBuffer *_buffer;

    struct {
        volatile bool execute;

        volatile bool deferred;
        volatile bool echo;
        volatile bool crc;
    } _flag{
        .execute = false,
        .deferred = false,
        .echo = false,
        .crc = true
    };

    void seek(Buffer::Offset offset);

    Response handle(bool initial, Value input) override;

    bool expectedInput(Value value) override;

    virtual void initTransfer();

    [[nodiscard]] virtual Buffer::BigSize bufferSize() const;

    virtual Response handleData(Buffer::Offset offset, Buffer::Byte input);
public:
    explicit InputCommand(ID id, InputBuffer *buffer);

    ~InputCommand() override;

    InputCommand *defer(bool deferred = true);

    virtual InputCommand *checksum(bool checksum = true);

    void loop() override; // needed only when deferred

    [[nodiscard]] InputBuffer *buffer() const;

    CRC::CRC16 crc();

    template<class B>
    InputCommand *grabBuffer(B *&buffer) {
        static_assert(std::is_base_of_v<InputBuffer, B>, "Derived not derived from Lib::SPI::Slave::InputBuffer");
        buffer = static_cast<B *>(_buffer);
        return this;
    }

    template<class B>
    InputCommand *withBuffer(const std::function<void(B &buffer)> &fn) {
        static_assert(std::is_base_of_v<InputBuffer, B>, "Derived not derived from Lib::SPI::Slave::InputBuffer");
        fn(_buffer);
        return this;
    }
};
}
#endif
