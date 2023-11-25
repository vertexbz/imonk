//
// Created by Adam Makswiej on 21/11/2023.
//

#include "Chronic.hpp"

Job::Chronic::Chronic(Job::Chronic::ms delay) {
    _delay = delay;
}

Job::Chronic::Chronic(Job::Chronic::Hz period) {
    _delay = (int16_t)1000 / (int16_t)period;
    _delay *= -1;
}

bool Job::Chronic::static_handler(struct repeating_timer *timer) {
    auto c = (Chronic *) timer->user_data;
    return c->handler();
}

bool Job::Chronic::handler() {
    run();
    return !_stopped;
}

bool Job::Chronic::begin() {
    if (!_stopped) {
        return true;
    }
    _stopped = false;
    return add_repeating_timer_ms(_delay, Chronic::static_handler, this, &_timer);
}

void Job::Chronic::stop() {
    _stopped = true;
}
