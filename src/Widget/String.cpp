//
// Created by Adam Makswiej on 21/11/2023.
//

#include "String.hpp"
#include "../Display/Painter.hpp"
#include "../Display/Util.hpp"

Widget::String::String(Display::Unit x, Display::Unit y, Display::Align align, Display::Color color, const Display::Font *font) {
    _x = x;
    _y = y;
    _align = align;
    _color = color;
    _font = font;
    _rendered = true;
    _string = "";
    _lastLength = 0;
}

void Widget::String::render(Display::Painter *painter) {
    if (_rendered) {
        return;
    }

    painter->setTextColor(_color);
    if (_lastLength) {
        Display::Unit x = _x;
        if (_align == Display::Align::Center) {
            x -= _lastLength / 2;
        } else if (_align == Display::Align::Right) {
            x -= _lastLength;
        }
        painter->fillRect(x, _y, _lastLength, Display::Util::fontHeight(_font), Display::Background);
    }
    _lastLength = painter->drawString(_string, _x, _y, _align, _font);
    _rendered = true;
}

void Widget::String::setString(::String &string) {
    if (_string.equals(string)) {
        return;
    }

    _string = std::move(string);
    _rendered = false;
}

void Widget::String::setString(const ::String &string) {
    if (_string.equals(string)) {
        return;
    }

    _string = string;
    _rendered = false;
}
