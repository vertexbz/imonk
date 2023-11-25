//
// Created by Adam Makswiej on 21/11/2023.
//

#include "State.hpp"
#include <Arduino.h>


State::State()
#ifdef SEM_STATE
: _sem(1, 1)
#endif
{

}

void State::begin() {
#ifndef SEM_STATE
    mutex_init(&_mutex);
#endif
}

void State::update(std::function<void(StateData *state)> updater) {
    noInterrupts();
#ifndef SEM_STATE
    mutex_enter_blocking(&_mutex);
#else
    _sem.acquire_blocking();
#endif
    updater(&_data);
#ifndef SEM_STATE
    mutex_exit(&_mutex);
#else
    _sem.release();
#endif
    interrupts();
}
