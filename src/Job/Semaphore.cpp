//
// Created by Adam Makswiej on 24/11/2023.
//

#include <api/Interrupts.h>
#include "Semaphore.hpp"

Job::Semaphore::Semaphore(Job::Semaphore::Count initial_permits, Job::Semaphore::Count max_permits) {
#ifndef SOFT_SEM
        sem_init(&_sem, initial_permits, max_permits);
#else
    _max = max_permits;
    _sem = initial_permits;
#endif
}

int Job::Semaphore::available() {
#ifndef SOFT_SEM
    return sem_available(&_sem);
#else
    return _sem;
#endif
}

bool Job::Semaphore::release() {
#ifndef SOFT_SEM
    return sem_release(&_sem);
#else
    if (_sem < _max) {
        ++_sem;
        return true;
    }
    return false;
#endif
}

void Job::Semaphore::acquire_blocking() {
#ifndef SOFT_SEM
    sem_acquire_blocking(&_sem);
#else
    while (_sem < 1) {
        for (uint8_t i = 0; i < 10; ++i) {
            yield();
            delay(1);
        }
    }
#endif
}
