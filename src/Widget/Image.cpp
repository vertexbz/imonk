//
// Created by Adam Makswiej on 21/11/2023.
//

#include "Image.hpp"

#include <Display/Painter.hpp>

Widget::Image::Image(Filesystem::File *file, const Display::Unit x, const Display::Unit y) {
    _rendered = false;
    _file = file;
    _x = x;
    _y = y;
}

void Widget::Image::render(Display::Painter *painter) {
    if (_rendered) {
        return;
    }
    painter->drawPng(_file, _x, _y);
    _rendered = true;
}
