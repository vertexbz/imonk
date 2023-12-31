# SPI Slave

Provides C++ abstraction over SPI Slave

## Usage
```c++
#include <vertaro/spi/slave.impl.hpp>

typedef struct {
    uint8_t chr;
    uint16_t word;
    bool tf;
} InterfaceData;

InterfaceData outputBuffer;
class Interface : public Lib::SPI::Slave::Interface {
public:
    Interface() : Lib::SPI::Slave::Interface() {
        addCommand(new Lib::SPI::Slave::OutputCommand<InterfaceData>(1));
        addCommand(new Lib::SPI::Slave::OutputCommand<uint16_t>(2));
        addCommand(new Lib::SPI::Slave::InputCommand<uint32_t>(10, 5));
        addCommand(new Lib::SPI::Slave::InputCommand<InterfaceData>(11));
        addCommand(new Lib::SPI::Slave::InputCommand<uint32_t>(12, 64));
        addCommand(new Lib::SPI::Slave::InputCommand<InterfaceData>(13, outputBuffer));
    }

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
};

Interface interface;

#include <Arduino.h>

void setup() {
    interface.setup();
}

void loop() {
    // Output data to master
    outputBuffer.chr = 7;
    outputBuffer.word = 8765;
    outputBuffer.tf = true;
    interface.output()->update(&outputBuffer);

    interface.output2()->update(1111);

    // Input data from master
    const InterfaceData &inputBuffer = interface.input()->value();

    uint32_t val = interface.input2()->value();
}
```