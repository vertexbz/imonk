#pragma once

#include "../Filesystem/File.hpp"
#include "../Display/types.hpp"
#include "../Display/Widget.hpp"

namespace Widget {
    class Image : public Display::Widget {
    protected:
        Display::Unit _x;
        Display::Unit _y;

        Filesystem::File *_file;
        bool _rendered;
    public:
        Image(Filesystem::File *file, Display::Unit x, Display::Unit y);

        void render(Display::Painter *) override;
    };
}
