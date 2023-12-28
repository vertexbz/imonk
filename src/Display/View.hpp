#pragma once

#include "types.hpp"

class StageManager;

namespace Display {
    class Painter;

    class View {
        StageManager *_manager = nullptr;
    protected:
        virtual void _init() {};
    public:
        void init(StageManager *manager) { _manager = manager; _init(); };
        virtual Color background() { return 0x000000; };
        virtual void render(Painter *) = 0;
        virtual ~View() = default;
    };
}
