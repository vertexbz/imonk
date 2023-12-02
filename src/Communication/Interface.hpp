#pragma once

#include <vertaro/spi/slave.impl.hpp>

namespace Communication {
class Interface final : protected Lib::SPI::Slave::Interface {
protected:
    Lib::SPI::Slave::VariableInputBuffer *_in_buf;

public:
    Interface();

    void begin();

    Lib::SPI::Slave::VariableInputBuffer *inBuf() {
        return _in_buf;
    }
};
}
