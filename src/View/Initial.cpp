//
// Created by Adam Makswiej on 20/11/2023.
//

#include "Initial.hpp"
#include "../Display/Display.hpp"
#include "../Filesystem/Filesystem.hpp"

View::Initial::Initial()
: line(120, 90, Display::Align::Center, 0XFFFFFF, &Display::Fonts::Font0)
, line2(120, 105, Display::Align::Center, 0XFF00FF, &Display::Fonts::Font2)
{
    line.setString("Initializing...");
    line2.setString(String("Flash Size: ") + Filesystem::Filesystem::getFlashSize() + "B"); // todo remove
}

void View::Initial::render(Display::Painter *painter) {
    line.render(painter);
    line2.render(painter);
}
