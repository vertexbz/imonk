//
// Created by Adam Makswiej on 2019-07-12.
//

#ifndef __SPISLAVELIB__COMMAND_HPP__
#define __SPISLAVELIB__COMMAND_HPP__

#include <cstdint>

namespace Lib::SPI::Slave {
class Interface;

class Command {
    friend class Interface;
public:
    typedef uint8_t ID;
    typedef uint8_t Value;

    typedef struct {
        Value value;
        bool next;
    } Response;

    static constexpr Value OK = 0xA5;
    static constexpr Value ok = 0xA4;
    static constexpr Value GoOn = 0x00;
    static constexpr Value BadCRC = 0x01;
    static constexpr Value Invalid = 0x02;
    static constexpr Value Empty = 0x03;
    static constexpr Value Repeat = 0xCC;
    static constexpr Value BadCall = 0xFF;
private:
    ID _id;

protected:
    virtual bool expectedInput(Value value);

    virtual Response handle(bool initial, Value value) = 0;

    static Response passTo(Command *command, bool initial, Value value);
public:
    explicit Command(ID id);

    virtual ~Command() = default;

    [[nodiscard]] ID id() const;

    virtual void setup();

    virtual void loop();
};
}
#endif
