#pragma once

#include "State.hpp"
#include "Display/Display.hpp"
#include "Scene/_scene_.hpp"
#include "Job/Chronic.hpp"

class StageManager : protected Job::Chronic {
protected:
    bool _newScene = false;
    Display::Display *_display;
    State *_state;

    std::unique_ptr<Scene::BaseScene> _scene;
public:
    StageManager(Hz hz, Display::Display *display, State *state);

    void enter(std::unique_ptr<Scene::BaseScene> scene);

protected:
    void run() override;
};
