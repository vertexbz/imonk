//
// Created by Adam Makswiej on 25/11/2023.
//
#include "StageManager.hpp"
#include "Filesystem/Filesystem.hpp"
#include "View/Custom.hpp"
#include "Widget/factory.hpp"
#include "Display/Util.hpp"
#include <ArduinoJson.h>

StageManager::StageManager(const Hz hz, Display::Display *display, Filesystem::Filesystem *filesystem)
: Chronic(hz),  _display(display), _filesystem(filesystem) {

}

void StageManager::enter(std::unique_ptr<View::BaseView> view) {
    _view = nullptr;
    _newView = true;
    _view = std::move(view);
    _view->init(this);
    begin();
}

StageManager::StageId StageManager::stagedId() const {
    return _staged_id;
}

StageManager::StageId StageManager::currentId() const {
    return _current_id;
}

void StageManager::forgetView(const char *name) {
    _custom_views.erase(const_cast<char *>(name));
}

void StageManager::loadView(const char *name) {
    // Open file for reading
    auto file = _filesystem->open(String(Filesystem::Filesystem::_VIEWS_) + "/" + name, "r");

    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use https://arduinojson.org/v6/assistant to compute the capacity.
    DynamicJsonDocument doc(1024);

    if (DeserializationError error = deserializeJson(doc, file)) {
        // todo display error screen when invalid file loaded
        return;
    }

    const auto background = Display::Util::strtorgb(doc["background"] | "000000");
    auto view = std::make_unique<View::Custom>(background);

    for( const auto& value : doc["widgets"].as<JsonArray>() ) {
        auto widget = build_widget(_filesystem, value.as<JsonObject>());

        // todo display error screen when invalid file loaded

        if (widget) {
            view->addWidget(std::move(widget));
        }
    }
    file.close();

    _custom_views[strdup(name)] = std::move(view);
}

bool StageManager::hasView(const char *name) const {
    return _custom_views.count(const_cast<char *>(name)) > 0;
}

bool StageManager::stage(const StageId id, const char *name) {
    if (!hasView(name)) {
        return false;
    }

    _staged_id = id;
    _staged = std::make_unique<View::Custom>(_custom_views[const_cast<char *>(name)].get());
    return true;
}

void StageManager::commit() {
    _current_id = _staged_id;
    _current = std::move(_staged);
    _current->init(this);
    _staged_id = 0;
    _staged = nullptr;
    _view = nullptr;

    _newView = true;
}

void StageManager::abort() {
    _staged = nullptr;
    _staged_id = 0;
}

void StageManager::run() {
    if (!_view && !_current) {
        return;
    }

    auto * view = _view.get();
    if (!view) {
        view = _current.get();
    }

    if (_newView) {
        _newView = false;
        _display->clearScreen(view->background());
    }
    _display->render(view);
}

