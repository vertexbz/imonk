//
// Created by Adam Makswiej on 21/11/2023.
//

#include "Image.hpp"

#include <Display/Painter.hpp>

Widget::Image::Image(std::shared_ptr<Filesystem::File> file, const Display::Unit x, const Display::Unit y) {
    _rendered = false;
    _file = std::move(file);
    _x = x;
    _y = y;
}

std::unique_ptr<Widget::BaseWidget> Widget::Image::clone() {
    return std::make_unique<Image>(_file, _x, _y);
}

void Widget::Image::setValue(TypeId type, void *data) {
}

Widget::BaseWidget::TypeId Widget::Image::typeId() {
    return TypeId::UNKNOWN;
}

void Widget::Image::render(Display::Painter *painter) {
    if (_rendered) {
        return;
    }

    _file->seek(0);
    painter->drawPng(_file.get(), _x, _y);
    _rendered = true;
}
