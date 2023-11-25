#include "StageManager.hpp"
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

void setup() {
    state.begin();
    flash.begin();
    //spi.begin();
    display.begin();

    manager.enter(std::make_unique<Scene::Initial>());
}

void setup1() {
    delay(1000);
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
    state.update([](StateData *state) {
        state->counter2 += 2;
        if (state->counter2 > 100) {
            state->counter2 = 0;
        }
    });
    state.update([](StateData *state){
        state->counter += 1;
        if (state->counter > 100) {
            state->counter = 0;
        }
    });
}
