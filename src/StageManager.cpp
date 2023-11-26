//
// Created by Adam Makswiej on 25/11/2023.
//
#include "StageManager.hpp"

StageManager::StageManager(const Hz hz, Display::Display *display, State *state) : Chronic(hz),  _display(display), _state(state) {}

void StageManager::enter(std::unique_ptr<Scene::BaseScene> scene) {
    _scene = nullptr;
    _newScene = true;
    _scene = std::move(scene);
    _scene->init(this);
    begin();
}

void StageManager::run() {
    if (!_scene) {
        return;
    }

    _state->update([this](StateData *state) {
        _scene->update(state);
    });
    if (_newScene) {
        _newScene = false;
        _display->clearScreen(_scene->background());
    }
    _display->render( _scene.get());
}

