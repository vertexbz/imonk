#pragma once

#include <FS.h>
#include "../Display/types.hpp"
#include "../Display/Widget.hpp"

namespace Widget {
    class Image : public Display::Widget {
    protected:
        Display::Unit _x;
        Display::Unit _y;

        File *_file;
        bool _rendered;
    public:
        Image(File *file, Display::Unit x, Display::Unit y);

        void render(Display::Painter *) override;
    };
}
