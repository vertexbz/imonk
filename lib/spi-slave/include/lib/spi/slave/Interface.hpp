//
// Created by Adam Makswiej on 2019-07-12.
//

#ifndef __SPISLAVELIB__SLAVE_HPP__
#define __SPISLAVELIB__SLAVE_HPP__

#include "Command.hpp"

namespace Lib::SPI::Slave {
class Interface {
    const uint8_t _iface;
    static void handler0();
    static void handler0_ss(unsigned int, uint32_t);
    static void handler1();
    static void handler1_ss(unsigned int, uint32_t);
protected:
    uint8_t _miso, _mosi, _sck, _ss;
    Command **_commands = nullptr;
    uint8_t _commandCount = 0;
    Command *_lastCommand = nullptr;

    virtual Command::Value execute(Command *command, bool initial, Command::Value value);

    virtual Command::Value interrupt(Command::Value value);

    Interface *addCommand(Command *command);

public:
    explicit Interface(uint8_t iface = 0);

    virtual void setup();

    virtual void end();

    virtual ~Interface();
};
}

#endif
