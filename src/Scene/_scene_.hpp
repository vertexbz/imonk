#pragma once

#include "../Display/Scene.hpp"
#include "../type.hpp"

namespace Scene {
    class BaseScene : public Display::Scene {

    public:
        virtual void update(StateData *state) {}

        Display::Color background() override {
            return 0x333333;
        }
    };
}
