#pragma once

#include "../Display/types.hpp"
#include "../Display/Widget.hpp"
#include "../Display/Sprite.hpp"

namespace Widget {
    class Gauge : public Display::Widget, protected Display::Sprite {
    public:
        typedef uint8_t Percentage;
    protected:
        Display::Unit _x;
        Display::Unit _y;

        Display::Unit _r;
        Display::Unit _thickness;

        Display::Color _color;

        float _rotation;
        float _gaugeArc;

        float _angle;
        float _angleRendered;

        Percentage _percentage;

        void _render();

        [[nodiscard]] float rotate(float angle) const;

    public:
        Gauge(Display::Unit x, Display::Unit y, Display::Unit radius, Display::Unit thickness, Display::Color color);

        Gauge(Display::Unit x, Display::Unit y, Display::Unit radius, Display::Unit thickness, Display::Color color, float rotation);

        Gauge(Display::Unit x, Display::Unit y, Display::Unit radius, Display::Unit thickness, Display::Color color, float rotation, float gaugeArc);

        void init() override;

        void render(Display::Painter *) override;

        void setPercent(Percentage percentage);
    };
}
