#pragma once

namespace Display {
    class Painter;

    class Widget {
    public:
        virtual void render(Painter *) = 0;
    };
}
