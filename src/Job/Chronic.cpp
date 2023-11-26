//
// Created by Adam Makswiej on 21/11/2023.
//

#include "Chronic.hpp"

Job::Chronic::Chronic(const ms delay) {
    _delay = delay;
}

Job::Chronic::Chronic(const Hz period) {
    _delay = static_cast<int16_t>(1000) / static_cast<int16_t>(period);
    _delay *= -1;
}

bool Job::Chronic::static_handler(repeating_timer *timer) {
    return static_cast<Chronic *>(timer->user_data)->handler();
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
    return add_repeating_timer_ms(_delay, static_handler, this, &_timer);
}

void Job::Chronic::stop() {
    _stopped = true;
}
