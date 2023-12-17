#pragma once

#include <pico/mutex.h>
#include "type.hpp"
#include <functional>

class State {
        mutex _mutex{nullptr};
    protected:
        StateData _data{};
    public:
        State();
        void begin();
        void update(const std::function<void(StateData *state)> &updater);
};
