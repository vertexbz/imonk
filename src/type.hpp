#pragma once

#include <cstdint>

struct StateData {
    volatile uint32_t counter3 = 0;
    volatile uint32_t counter2 = 0;
    volatile uint32_t counter = 0;
};
