//
// Created by Adam Makswiej on 20/11/2023.
//

#include "Custom.hpp"
#include "../Display/Display.hpp"

void View::Custom::_init() {
    for (auto & widget : _widgets) {
        widget->init();
    }
}

View::Custom::Custom(const Custom *original) {
    _background = original->_background;
    for (auto & widget : original->_widgets) {
        _widgets.push_back(std::move(widget->clone()));
    }
}

View::Custom::Custom(const Display::Color background) {
    _background = background;
}

void View::Custom::addWidget(std::unique_ptr<Widget::BaseWidget> widget) {
    _widgets.push_back(std::move(widget));
}

Display::Color View::Custom::background() {
    return _background;
}

void View::Custom::render(Display::Painter *painter) {
    for (auto & widget : _widgets) {
        widget->render(painter);
    }
}

Widget::BaseWidget* View::Custom::slot(SlotId id) {
    SlotId current = 0;
    for (auto & widget : _widgets) {
        if (widget->typeId() != Widget::BaseWidget::TypeId::UNKNOWN && id == current++) {
            return widget.get();
        }
    }
    return nullptr;
}

View::BaseView::SlotId View::Custom::slots() {
    SlotId current = 0;
    for (auto & widget : _widgets) {
        if (widget->typeId() != Widget::BaseWidget::TypeId::UNKNOWN) {
            current++;
        }
    }

    return current;
}
