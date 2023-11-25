#pragma once

#include "Chronic.hpp"
#include <functional>

namespace Job {
    class ChronicFunc : public Chronic {
    public:
        typedef std::function<void()> Handler;
    protected:
        Handler _handler;
    public:
        explicit ChronicFunc(ms delay, Job::ChronicFunc::Handler handler);

        explicit ChronicFunc(Hz period, Job::ChronicFunc::Handler handler);

    protected:
        void run() override;
    };
}
