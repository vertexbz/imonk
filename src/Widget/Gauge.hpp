#pragma once

#include "_widget_.hpp"
#include "../Display/Sprite.hpp"

namespace Widget {
    class Gauge : public BaseWidget, protected Display::Sprite {
    public:
        typedef uint8_t Percentage;
    protected:
        Display::Unit _x;
        Display::Unit _y;

        Display::Unit _r;
        Display::Unit _thickness;

        Display::Color _color;
        Display::Color _background;

        float _rotation;
        float _gaugeArc;

        float _angle;
        float _angleRendered;

        Percentage _percentage;

        void _render();

        [[nodiscard]] float rotate(float angle) const;

    public:
        Gauge(Display::Unit x, Display::Unit y, Display::Unit radius, Display::Unit thickness, Display::Color color);

        Gauge(Display::Unit x, Display::Unit y, Display::Unit radius, Display::Unit thickness, Display::Color color, Display::Color background);

        Gauge(Display::Unit x, Display::Unit y, Display::Unit radius, Display::Unit thickness, Display::Color color, Display::Color background, float rotation);

        Gauge(Display::Unit x, Display::Unit y, Display::Unit radius, Display::Unit thickness, Display::Color color, Display::Color background, float rotation, float gaugeArc);

        void init() override;

        void render(Painter *) override;

        void setPercent(Percentage percentage);

        std::unique_ptr<BaseWidget> clone() override;
        void setValue(TypeId type, void *data) override;

        TypeId typeId() override;
    };
}
