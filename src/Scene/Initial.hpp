#pragma once
#include "_scene_.hpp"
#include "../Widget/Gauge.hpp"
#include "../Widget/String.hpp"

namespace Scene {
    class Initial : public BaseScene {
    private:
        Widget::Gauge gauge;
        Widget::Gauge gauge2;
        Widget::Gauge gauge3;
        Widget::String line;
        Widget::String line2;
        Widget::String line3;

    protected:
        void _init() override;
    public:
        Initial();
        void render(Display::Painter *painter) override;

        void update(StateData *state) override;

    };
}
