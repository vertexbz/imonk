#include "StageManager.hpp"
#include "Communication/Interface.hpp"
#include "Filesystem/Filesystem.hpp"
#include "Display/Display.hpp"
#include "State.hpp"

#include <Scene/Initial.hpp>
//#include <Scene/FatalError.hpp>
//#include <Scene/FirmwareUpgrade.hpp>



static Display::Display display;
static Filesystem::Filesystem flash;
static State state;
static StageManager manager(24, &display, &state);

static Communication::Interface interface(&flash);

const char face1[] = "/images/face.png";
const char face2[] = "/images/face2.png";
void setup() {
    state.begin();
    flash.begin();
    display.begin();

    const char *face_path = face1;
    if (flash.exists(face2)) {
        face_path = face2;
    }

    static File face = flash.open(face_path, "r");

    auto scene = std::make_unique<Scene::Initial>(&face);
    interface.inBuf()->callback([](Lib::SPI::Slave::Buffer::Data data, Lib::SPI::Slave::Buffer::Size size, void* scene) {
        static_cast<Scene::Initial*>(scene)->l2()->setString(String(data, size));
    }, scene.get());

    manager.enter(std::move(scene));
}

void setup1() {
    interface.begin();
}

void loop() {
    interface.loop();
    state.update([](StateData *state){
        if (const int8_t progress = interface.upImg()->data()->progress(); progress >= 0) {
            state->counter3 = static_cast<uint8_t>(progress);
        } else {
            state->counter3 = 0;
        }
    });
}

void loop1() {


}
