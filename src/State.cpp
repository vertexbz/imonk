//
// Created by Adam Makswiej on 21/11/2023.
//

#include "State.hpp"
#include <Arduino.h>

State::State() {}

void State::begin() {
    mutex_init(&_mutex);
}

void State::update(const std::function<void(StateData *state)> &updater) {
    noInterrupts();
    mutex_enter_blocking(&_mutex);
    updater(&_data);
    mutex_exit(&_mutex);
    interrupts();
}
