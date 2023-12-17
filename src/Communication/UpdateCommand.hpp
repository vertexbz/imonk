#pragma once

#include "../Filesystem/Filesystem.hpp"
#include <vertaro/spi/slave.hpp>


namespace Communication {
class UpdateCommand : public Lib::SPI::Slave::NamedUploadCommand {
protected:
    void initUpdate(Filesystem::Filesystem *fs);
public:
    UpdateCommand(ID id, Filesystem::Filesystem *fs);
};
}
