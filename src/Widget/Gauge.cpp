//
// Created by Adam Makswiej on 21/11/2023.
//

#include "../Display/Painter.hpp"
#include "../Display/Display.hpp"
#include "Gauge.hpp"

Widget::Gauge::Gauge(Display::Unit x, Display::Unit y, Display::Unit radius, Display::Unit thickness, Display::Color color)
: Gauge(x, y, radius, thickness, color, 0x000000) {}

Widget::Gauge::Gauge(Display::Unit x, Display::Unit y, Display::Unit radius, Display::Unit thickness, Display::Color color, Display::Color background)
: Gauge(x, y, radius, thickness, color, background, 90) {}

Widget::Gauge::Gauge(Display::Unit x, Display::Unit y, Display::Unit radius, Display::Unit thickness, Display::Color color, Display::Color background, float rotation)
: Gauge(x, y, radius, thickness, color, background, rotation, 260) {}

Widget::Gauge::Gauge(Display::Unit x, Display::Unit y, Display::Unit radius, Display::Unit thickness, Display::Color color, Display::Color background, float rotation, float gaugeArc)
: Sprite(radius * 2 + 2, radius * 2 + 2) {
    _percentage = 0;

    _y = y;
    _x = x;
    _r = radius;
    _thickness = thickness;
    _color = color;
    _background = background;
    _rotation = rotation;
    _gaugeArc = gaugeArc;

    _angle = rotate(_percentage);
    _angleRendered = rotate(0);
}

std::unique_ptr<Widget::BaseWidget> Widget::Gauge::clone() {
    auto gauge = std::make_unique<Gauge>(_x, _y, _r, _thickness, _color, _background, _rotation, _gaugeArc);
    gauge->setPercent(_percentage);
    return std::move(gauge);
}

void Widget::Gauge::setValue(TypeId type, void *data) {
    setPercent(*static_cast<int32_t*>(data));
}

Widget::BaseWidget::TypeId Widget::Gauge::typeId() {
    return TypeId::INTEGER;
}

void Widget::Gauge::init() {
    BaseWidget::init();
    Sprite::init();
    setColorDepth(Display::ColorDepth::grayscale_2bit);
    setPaletteColor(0, (_color & 0x010101) == 0x010101 ?  (_color & ~0x010101) : (_color | 0x010101));
    setPaletteColor(1, _background);
    setPaletteColor(2, _color);

    setPivot(0,0);
    fillScreen(0);
    fillArc(_r + 1, _r + 1, _r - _thickness, _r, rotate(0), rotate(_gaugeArc), 1);

    _render();
}

void Widget::Gauge::setPercent(Gauge::Percentage percentage) {
    if (_percentage == percentage) {
        return;
    }

    float angle = (float) min(percentage, 100) * (_gaugeArc / 100);
    _angle = rotate(angle);
    _percentage = percentage;
}

void Widget::Gauge::render(Painter *painter) {
    if (_angleRendered == _angle) {
        return;
    }

    _render();
    painter->renderSprite(this,  _x - _r - 1, _y - _r - 1, 0);
    _angleRendered = _angle;
}

void Widget::Gauge::_render() {
    if (_angleRendered > _angle) {
        fillArc(_r + 1, _r + 1, _r - _thickness, _r, _angle, _angleRendered + 1, 1);
    } else if (_angle > _angleRendered) {
        fillArc(_r + 1, _r + 1, _r - _thickness, _r, _angleRendered - 1, _angle, 2);
    }
}

float Widget::Gauge::rotate(float angle) const {
    float rotation = _rotation + (360 - _gaugeArc) / 2;
    return max(0, min(_gaugeArc, angle)) + rotation;
}
