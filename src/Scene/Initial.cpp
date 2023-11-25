//
// Created by Adam Makswiej on 20/11/2023.
//

#include "Initial.hpp"
#include "../Display/Display.hpp"
#include "../Filesystem/Filesystem.hpp"
#include <string>

Scene::Initial::Initial()
: gauge(120, 120, 120, 9, 0xFF0000)
, gauge2(120, 120, 110, 9, 0xFF8800)
, gauge3(120, 120, 100, 9, 0x0000FF)
, line(120, 90, Display::Align::Center, 0xffffff, &Display::Fonts::Font0)
, line2(120, 110, Display::Align::Center, 0xff00ff, &Display::Fonts::Font0)
, line3(120, 130, Display::Align::Center, 0xffff00, &Display::Fonts::Font2)
{
    line.setString("Initializing...");
    line2.setString(String("Flash Size: ") + Filesystem::Filesystem::getFlashSize() + "b");
}

void Scene::Initial::_init() {
    gauge.init();
    gauge2.init();
    gauge3.init();
}

void Scene::Initial::update(StateData *state) {
    gauge.setPercent(state->counter);
    gauge2.setPercent(state->counter2);
    gauge3.setPercent(state->counter3);
    line3.setString(String("Count: ") + state->counter + " / 100");
}

void Scene::Initial::render(Display::Painter *painter) {
    line.render(painter);
    line2.render(painter);
    line3.render(painter);
    gauge.render(painter);
    gauge2.render(painter);
    gauge3.render(painter);
}
