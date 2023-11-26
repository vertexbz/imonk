#pragma once

#include <cstdint>

#define INTERFACE 0

namespace Communication {
    class Interface {
    private:
        static void handler0();
        static void handler1();
    protected:
        virtual uint8_t process(uint8_t byte);
    public:
        Interface();
        void begin();
    };
}