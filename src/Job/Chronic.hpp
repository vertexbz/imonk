#pragma once

#include <pico/time.h>

namespace Job {
    class Chronic {
    public:
        typedef uint16_t Hz;
        typedef int32_t ms;
    private:
        ms _delay;
        repeating_timer _timer{0};
        volatile bool _stopped = true;
    protected:
        static bool static_handler(struct repeating_timer *timer);

        virtual bool handler();

        virtual void run() = 0;

    public:
        explicit Chronic(ms delay);

        explicit Chronic(Hz period);

        bool begin();

        void stop();
    };
}