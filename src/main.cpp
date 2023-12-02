#include "StageManager.hpp"
#include "Job/Semaphore.hpp"
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

static Communication::Interface interface;

static Job::Semaphore init_holder(0, 1);

void setup() {
    state.begin();
    flash.begin();
    display.begin();

    if (flash.exists("/images/face.png")) {
        state.update([](StateData *state) {
            state->counter3 = 50;
        });
    }

    static File face = flash.open("/images/face.png", "r");

    auto scene = std::make_unique<Scene::Initial>(&face);
    interface.inBuf()->callback([](Lib::SPI::Slave::Buffer::Data data, Lib::SPI::Slave::Buffer::Size size, void* scene) {
        static_cast<Scene::Initial*>(scene)->l2()->setString(String(data, size));
    }, scene.get());

    manager.enter(std::move(scene));

    init_holder.release();
}

void setup1() {
    init_holder.acquire_blocking();

    interface.begin();


    delay(1000); // todo
}

void loop() {
    delay(45);
    state.update([](StateData *state){
        state->counter3 += 2;
        if (state->counter3 > 100) {
            state->counter3 = 0;
        }
    });
}

void loop1() {
    delay(90);
    // state.update([](StateData *state) {
    //     state->counter2 += 2;
    //     if (state->counter2 > 100) {
    //         state->counter2 = 0;
    //     }
    // });
    // state.update([](StateData *state){
    //     state->counter += 1;
    //     if (state->counter > 100) {
    //         state->counter = 0;
    //     }
    // });
}
