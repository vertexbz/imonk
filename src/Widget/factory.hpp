#pragma once

#include "Gauge.hpp"
#include "Image.hpp"
#include "String.hpp"

#include<memory>
#include <ArduinoJson.hpp>
#include <Display/Util.hpp>


inline std::unique_ptr<Widget::BaseWidget> build_widget(Filesystem::Filesystem *fs, const ArduinoJson::JsonObject obj) {
    if (strcmp(obj["type"].as<const char*>(), "string") == 0) {
        auto str = std::make_unique<Widget::String>(
            obj["x"].as<Display::Unit>(), obj["y"].as<Display::Unit>(),
           Display::Util::strtoalign(obj["align"] | "center"),
            Display::Util::strtorgb(obj["color"]),
            Display::Util::strtofont(obj["font"] | "font0")
        );
        str->setString(obj["value"].as<const char *>());
        return std::move(str);
    }
    if (strcmp(obj["type"].as<const char*>(), "gauge") == 0) {
        auto gauge = std::make_unique<Widget::Gauge>(
            obj["x"].as<Display::Unit>(), obj["y"].as<Display::Unit>(),
            obj["r-end"].as<Display::Unit>(), obj["r-end"].as<Display::Unit>() - obj["r-start"].as<Display::Unit>(),
            Display::Util::strtorgb(obj["color"]),
            Display::Util::strtorgb(obj["background"]),
            obj["rotation"] | 90.0,
            obj["arc"] | 260.0
        );
        gauge->setPercent(obj["value"].as<uint8_t>());
        return std::move(gauge);
    }
    if (strcmp(obj["type"].as<const char*>(), "image") == 0) {
        auto gauge = std::make_unique<Widget::Image>(
            fs->openPtr(String(Filesystem::Filesystem::_IMAGES_) + obj["name"].as<const char *>(), "r"),
            obj["x"].as<Display::Unit>(), obj["y"].as<Display::Unit>()
        );
        return std::move(gauge);
    }
    return nullptr;
}
