#pragma once

namespace Display {
    class Painter;

    class Widget {
    public:
        virtual ~Widget() = default;

        virtual void render(Painter *) = 0;
    };
}
