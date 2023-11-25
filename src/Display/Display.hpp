#pragma once

#include <LGFX_GC9A01.hpp>
#include "Painter.hpp"
#include "Sprite.hpp"

namespace Display {
    class Scene;

    class Display : private LGFX_GC9A01, public Painter {
        friend class Sprite;
    protected:
        Sprite _canvas;
    public:
        Display();

        bool begin();

        void render(Scene *scene);

#pragma mark Painter - Info

        [[nodiscard]] Unit width() const override;

        [[nodiscard]] Unit height() const override;

        [[nodiscard]] float pivotX() const override;

        [[nodiscard]] float pivotY() const override;

#pragma mark Painter - Pixel
        void drawPixel(Unit x, Unit y) override;

#pragma mark Painter - Arc
        using Painter::fillArc;
        void fillArc(Unit x, Unit y, Unit r0, Unit r1, float angle0, float angle1) override;

#pragma mark Painter - Line

        void drawFastVLine(Unit x, Unit y, Unit h) override;

        void drawFastHLine(Unit x, Unit y, Unit w) override;

#pragma mark Painter - Rect
        using Painter::drawRect;

        void drawRect(Unit x, Unit y, Unit w, Unit h) override;

        using Painter::fillRect;
        void fillRect(Unit x, Unit y, Unit w, Unit h) override;

#pragma mark Painter - Misc

        void setPivot(float x, float y) override;

        void setColor(Color color) override;

        void setTextColor(Color color) override;

        void setColorDepth(ColorDepth depth) override;

        void setPaletteColor(uint8_t index, Color color) override;

        void fillScreen(Color color) override;

#pragma mark Painter - String

        size_t drawString(const char *string, Unit x, Unit y, Align align, const Font *font) override;

#pragma mark Painter - Render
        using Painter::renderSprite;
        void renderSprite(Sprite *sprite, Unit x, Unit y, Color transp) override;

        using Painter::renderRotated;
        using Painter::renderRotateZoom;
        void renderRotateZoom(Sprite *sprite, float x, float y, float angle, float zoom_x, float zoom_y, Color transp) override;

        using Painter::renderRotatedAA;
        using Painter::renderRotateZoomAA;
        void renderRotateZoomAA(Sprite *sprite, float x, float y, float angle, float zoom_x, float zoom_y, Color transp) override;

        void renderAffine(Sprite *sprite, const float *matrix, Color transp) override;
        void renderAffineAA(Sprite *sprite, const float *matrix, Color transp) override;
    };
}
