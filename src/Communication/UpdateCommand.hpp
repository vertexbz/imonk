#pragma once

#include <vertaro/spi/slave.hpp>


namespace Communication {
class UpdateCommand : public Lib::SPI::Slave::NamedUploadCommand {
protected:
    void initUpdate(FS *fs);
public:
    UpdateCommand(ID id, FS *fs);
};
}
