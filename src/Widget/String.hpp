#pragma once
#include "../Display/Widget.hpp"
#include "../Display/types.hpp"
#include <StreamString.h>

namespace Widget {
    class String : public Display::Widget {
    protected:
        Display::Unit _x;
        Display::Unit _y;
        Display::Unit _lastLength;

        Display::Align _align;
        Display::Color _color;
        const Display::Font *_font;

        ::String _string;

        bool _rendered;
    public:
        String(Display::Unit x, Display::Unit y, Display::Align align, Display::Color color, const Display::Font* font);
        void setString(::String &string);
        void setString(const ::String &string);
        void render(Display::Painter *) override;
    };
}
