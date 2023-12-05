#pragma once

#include <vertaro/spi/slave.impl.hpp>

namespace Filesystem {
class Filesystem;
}

namespace Communication {
class Interface final : protected Lib::SPI::Slave::Interface {
protected:
    Lib::SPI::Slave::VariableInputBuffer *_in_buf = nullptr;

public:
    explicit Interface(Filesystem::Filesystem *fs);

    void begin();

    Lib::SPI::Slave::VariableInputBuffer *inBuf() {
        return _in_buf;
    }
};
}
