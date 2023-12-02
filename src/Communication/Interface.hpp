#pragma once

#include <vertaro/spi/slave.impl.hpp>


typedef struct {
    uint8_t chr;
    uint16_t word;
    bool tf;
} __attribute__ ((aligned(1), packed)) InterfaceData;

namespace Communication {
class Interface final : protected Lib::SPI::Slave::Interface {
protected:
    Lib::SPI::Slave::VariableInputBuffer *_in_buf;
    Lib::SPI::Slave::VariableOutputBuffer *_out_buf;

public:
    Interface();

    void begin();

    Lib::SPI::Slave::OutputCommand<InterfaceData> *output() {
        return static_cast<Lib::SPI::Slave::OutputCommand<InterfaceData> *>(_commands[0]);
    }

    Lib::SPI::Slave::OutputCommand<uint16_t> *output2() {
        return static_cast<Lib::SPI::Slave::OutputCommand<uint16_t> *>(_commands[1]);
    }

    Lib::SPI::Slave::InputCommand<InterfaceData> *input() {
        return static_cast<Lib::SPI::Slave::InputCommand<InterfaceData> *>(_commands[2]);
    }

    Lib::SPI::Slave::InputCommand<uint32_t> *input2() {
        return static_cast<Lib::SPI::Slave::InputCommand<uint32_t> *>(_commands[3]);
    }

    Lib::SPI::Slave::VariableInputBuffer *inBuf() {
        return _in_buf;
    }

    Lib::SPI::Slave::VarOutputCommand *varoutput() {
        return static_cast<Lib::SPI::Slave::VarOutputCommand *>(_commands[7]);
    }
};
}
