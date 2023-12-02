#pragma once

#include <vertaro/spi/slave.impl.hpp>

namespace Communication {
class Interface final : protected Lib::SPI::Slave::Interface {
protected:
    Lib::SPI::Slave::VariableInputBuffer *_in_buf;
    Lib::SPI::Slave::VariableOutputBuffer *_out_buf;

public:
    Interface();

    void begin();

    Lib::SPI::Slave::VariableInputBuffer *inBuf() {
        return _in_buf;
    }

    Lib::SPI::Slave::VarOutputCommand *varoutput() {
        return static_cast<Lib::SPI::Slave::VarOutputCommand *>(_commands[7]);
    }
};
}
