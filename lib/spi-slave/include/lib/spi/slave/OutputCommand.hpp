//
// Created by Adam Makswiej on 2019-07-12.
//

#ifndef __SPISLAVELIB__SPISLAVE_COMMAND_OUTPUT_HPP__
#define __SPISLAVELIB__SPISLAVE_COMMAND_OUTPUT_HPP__

#include "Command.hpp"
#include "./buffer/OutputBuffer.hpp"
#include <type_traits>
#include <functional>

namespace Lib::SPI::Slave {
class OutputCommand : public Command {
protected:
    volatile Buffer::Offset _position = 0;
    volatile uint16_t _crc = 0;
    OutputBuffer *_buffer;

    volatile struct {
        volatile bool execute;

        volatile bool deferred;
    } _flag {
        .execute = false,
        .deferred = false
    };

    Response handle(bool initial, Value value) override;

    bool expectedInput(Value value) override;

public:
    explicit OutputCommand(ID id, OutputBuffer *buffer);

    ~OutputCommand() override;

    void loop() override;

    OutputCommand *defer(bool deferred = true);

    [[nodiscard]] OutputBuffer *buffer() const;

    template<class B>
    OutputCommand *grabBuffer(B *&buffer) {
        static_assert(std::is_base_of_v<OutputBuffer, B>, "Derived not derived from Lib::SPI::Slave::OutputBuffer");
        buffer = static_cast<B *>(_buffer);
        return this;
    }

    template<class B>
    OutputCommand *withBuffer(const std::function<void(B &buffer)> &fn) {
        static_assert(std::is_base_of_v<OutputBuffer, B>, "Derived not derived from Lib::SPI::Slave::OutputBuffer");
        fn(*static_cast<B*>(_buffer));
        return this;
    }
};
}
#endif
