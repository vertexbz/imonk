#pragma once

//#define SEM_STATE

#ifndef SEM_STATE
    #include <pico/mutex.h>
#else
    #include "Job/Semaphore.hpp"
#endif
#include "type.hpp"
#include <functional>

class State {
    private:
#ifndef SEM_STATE
        mutex _mutex{nullptr};
#else
        Job::Semaphore _sem;
#endif
    protected:
        StateData _data{};
    public:
        State();
        void begin();
        void update(std::function<void(StateData *state)> updater);
};
