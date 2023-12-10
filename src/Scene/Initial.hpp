#pragma once
#include "_scene_.hpp"
#include "../Filesystem/File.hpp"
#include "../Widget/Gauge.hpp"
#include "../Widget/String.hpp"
#include "../Widget/Image.hpp"

namespace Scene {
    class Initial : public BaseScene {
        Widget::Gauge gauge;
        Widget::Gauge gauge2;
        Widget::Gauge gauge3;
        Widget::String line;
        Widget::String line2;
        Widget::String line3;
        Widget::Image face;

    protected:
        void _init() override;
    public:
        explicit Initial(Filesystem::File *face);
        void render(Display::Painter *painter) override;

        void update(StateData *state) override;

        Widget::String *l2() {
            return &line2;
        }
    };
}
