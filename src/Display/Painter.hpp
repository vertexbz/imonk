#pragma once

#include "types.hpp"

namespace Display {
    class Display;
    class Sprite;

    class Painter {
    protected:
        [[nodiscard]] virtual const Font* defaultFont() const { return &Fonts::Font0; };

    public:
#pragma mark Info
        [[nodiscard]] virtual Unit width() const = 0;
        [[nodiscard]] virtual Unit height() const = 0;
        [[nodiscard]] virtual float pivotX() const = 0;
        [[nodiscard]] virtual float pivotY() const = 0;

#pragma mark Misc
        virtual void setPivot(float x, float y) = 0;
        virtual void setColor(Color color) = 0;
        virtual void setTextColor(Color color) = 0;
        virtual void setPaletteColor(uint8_t index, Color color) = 0;
        virtual void setColorDepth(ColorDepth depth) = 0;
        virtual void fillScreen(Color color) = 0;
        virtual void clearScreen(Color color) = 0;

#pragma mark Pixel
        inline void drawPixel(Unit x, Unit y, Color color) { setColor(color); drawPixel(x, y); }
        virtual void drawPixel(Unit x, Unit y) = 0;

#pragma mark Arc
        inline void fillArc(Unit x, Unit y, Unit r0, Unit r1, float angle0, float angle1, Color color) { setColor(color); fillArc(x, y, r0, r1, angle0, angle1); }
        virtual void fillArc(Unit x, Unit y, Unit r0, Unit r1, float angle0, float angle1) = 0;

#pragma mark Line
        inline void drawFastVLine(Unit x, Unit y, Unit h, Color color) { setColor(color); drawFastVLine(x, y, h);}
        virtual void drawFastVLine(Unit x, Unit y, Unit h) = 0;

        inline void drawFastHLine(Unit x, Unit y, Unit w, Color color) { setColor(color); drawFastHLine(x, y, w);}
        virtual void drawFastHLine(Unit x, Unit y, Unit w) = 0;

#pragma mark Line
        inline void drawRect(Unit x, Unit y, Unit w, Unit h, Color color) { setColor(color); drawRect(x, y, w, h);}
        virtual void drawRect(Unit x, Unit y, Unit w, Unit h) = 0;
        inline void fillRect(Unit x, Unit y, Unit w, Unit h, Color color) { setColor(color); fillRect(x, y, w, h);}
        virtual void fillRect(Unit x, Unit y, Unit w, Unit h) = 0;

#pragma mark String
        inline size_t drawString(const String &string, Unit x, Unit y) { return drawString(string.c_str(), x, y, Align::Left, defaultFont()); };
        inline size_t drawString(const String &string, Unit x, Unit y, const Font* font) { return drawString(string.c_str(), x, y, Align::Left, font); };
        inline size_t drawString(const String &string, Unit x, Unit y, Align align) { return drawString(string.c_str(), x, y, align, defaultFont()); };
        inline size_t drawString(const String &string, Unit x, Unit y, Align align, const Font* font) { return drawString(string.c_str(), x, y, align, font); };

        inline size_t drawString(const char *string, Unit x, Unit y) { return drawString(string, x, y, Align::Left, defaultFont()); };
        inline size_t drawString(const char *string, Unit x, Unit y, const Font* font) { return drawString(string, x, y, Align::Left, font); };
        inline size_t drawString(const char *string, Unit x, Unit y, Align align) { return drawString(string, x, y, align, defaultFont()); };
        virtual size_t drawString(const char *string, Unit x, Unit y, Align align, const Font* font) = 0;

#pragma mark Images
        inline bool drawPng(File *file) { return drawPng(file, 0, 0); };
        inline bool drawPng(File *file, Unit x, Unit y) { return drawPng(file, x, y, 0, 0); };
        inline bool drawPng(File *file, Unit x, Unit y, Unit maxWidth, Unit maxHeight) { return drawPng(file, x, y, maxWidth, maxHeight, 0, 0, 1.f, 1.f); };
        inline bool drawPng(File *file, Unit x, Unit y, Unit maxWidth, Unit maxHeight, Unit offX, Unit offY, float scaleX, float scaleY) { return drawPng(file, x, y,  maxWidth, maxHeight, offX, offY, scaleX, scaleY, Align2D::TopLeft); };
        virtual bool drawPng(File *file, Unit x, Unit y, Unit maxWidth, Unit maxHeight, Unit offX, Unit offY, float scaleX, float scaleY, Align2D align) = 0;

#pragma mark Render
        inline void renderSprite(Sprite *sprite, Unit x, Unit y) { renderSprite(sprite, x, y, NoTransparent); };
        virtual void renderSprite(Sprite *sprite, Unit x, Unit y, Color transp) = 0;

        inline void renderRotated(Sprite *sprite, float angle)  { renderRotated(sprite, angle, NoTransparent); };
        inline void renderRotated(Sprite *sprite, float angle, Color transp) { renderRotateZoom(sprite, angle, 1.0f, 1.0f, transp); };
        inline void renderRotateZoom(Sprite *sprite, float angle, float zoom_x, float zoom_y) { renderRotateZoom(sprite, angle, zoom_x, zoom_y, NoTransparent); };
        inline void renderRotateZoom(Sprite *sprite, float angle, float zoom_x, float zoom_y, Color transp) { renderRotateZoom(sprite, pivotX(), pivotY(), angle, zoom_x, zoom_y, transp); };
        inline void renderRotateZoom(Sprite *sprite, float x, float y, float angle, float zoom_x, float zoom_y) { renderRotateZoom(sprite, x, y, angle, zoom_x, zoom_y, NoTransparent); };
        virtual void renderRotateZoom(Sprite *sprite, float x, float y, float angle, float zoom_x, float zoom_y, Color transp) = 0;

        inline void renderRotatedAA(Sprite *sprite, float angle)  { renderRotatedAA(sprite, angle, NoTransparent); };
        inline void renderRotatedAA(Sprite *sprite, float angle, Color transp) { renderRotateZoomAA(sprite, angle, 1.0f, 1.0f, transp); };
        inline void renderRotateZoomAA(Sprite *sprite, float angle, float zoom_x, float zoom_y) { renderRotateZoomAA(sprite, angle, zoom_x, zoom_y, NoTransparent); };
        inline void renderRotateZoomAA(Sprite *sprite, float angle, float zoom_x, float zoom_y, Color transp) { renderRotateZoomAA(sprite, pivotX(), pivotY(), angle, zoom_x, zoom_y, transp); };
        inline void renderRotateZoomAA(Sprite *sprite, float x, float y, float angle, float zoom_x, float zoom_y) { renderRotateZoomAA(sprite, x, y, angle, zoom_x, zoom_y, NoTransparent); };
        virtual void renderRotateZoomAA(Sprite *sprite, float x, float y, float angle, float zoom_x, float zoom_y, Color transp) = 0;

        inline void renderAffine(Sprite *sprite, const float *matrix) { renderAffine(sprite, matrix, NoTransparent); };
        virtual void renderAffine(Sprite *sprite, const float *matrix, Color transp) = 0;

        inline void renderAffineAA(Sprite *sprite, const float *matrix) { renderAffineAA(sprite, matrix, NoTransparent); };
        virtual void renderAffineAA(Sprite *sprite, const float *matrix, Color transp) = 0;


    };
}
