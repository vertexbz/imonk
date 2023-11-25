//
// Created by Adam Makswiej on 21/11/2023.
//

#include "ChronicFunc.hpp"
#include <utility>

Job::ChronicFunc::ChronicFunc(Job::Chronic::ms delay, Job::ChronicFunc::Handler handler) : Chronic(delay) {
    _handler = std::move(handler);
}

Job::ChronicFunc::ChronicFunc(Job::Chronic::Hz period, Job::ChronicFunc::Handler handler) : Chronic(period) {
    _handler = std::move(handler);
}

void Job::ChronicFunc::run() {
    _handler();
}
