#include "StageManager.hpp"
#include "Communication/Interface.hpp"
#include "Filesystem/Filesystem.hpp"
#include "Display/Display.hpp"
#include "View/Initial.hpp"

static Display::Display display;
static Filesystem::Filesystem flash;
static StageManager manager(24, &display, &flash);
static Communication::Interface interface(&flash, &manager);

void setup() {
    flash.begin();
    display.begin();

    auto dir = flash.openDir(Filesystem::Filesystem::_VIEWS_);
    while (dir.next()) {
        if (dir.isFile()) {
            manager.loadView(dir.fileName().c_str());
        }
    }

    manager.enter(std::make_unique<View::Initial>());
}

void setup1() {
    interface.begin();
}

void loop() {
    interface.loop();
}

void loop1() {

}
