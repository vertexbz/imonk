//
// Created by Adam Makswiej on 21/11/2023.
//

#include "Display.hpp"
#include "Painter.hpp"
#include "Sprite.hpp"

Display::Sprite::Sprite(Unit width, Unit height) : LGFX_Sprite() {
    _width = width;
    _height = height;
}


void Display::Sprite::init() {
    setPsram(true);
    createSprite(_width, _height);
}


#pragma mark Painter - Info

Display::Unit Display::Sprite::width() const {
    return _width;
}

Display::Unit Display::Sprite::height() const {
    return _height;
}

float Display::Sprite::pivotX() const {
    return _xpivot;
}

float Display::Sprite::pivotY() const {
    return _ypivot;
}

#pragma mark Painter - Misc
void Display::Sprite::setPivot(float x, float y) {
    LGFX_Sprite::setPivot(x, y);
}

void Display::Sprite::setColor(Color color) {
    if (color == Background) {
        color = _text_style.back_rgb888;
    }
    LGFX_Sprite::setColor(color);
}

void Display::Sprite::setTextColor(Color color) {
    if (color == Background) {
        color = _text_style.back_rgb888;
    }
    LGFX_Sprite::setTextColor(color, _text_style.back_rgb888);
}

void Display::Sprite::setColorDepth(ColorDepth depth) {
    LGFX_Sprite::setColorDepth((uint8_t)depth);
}

void Display::Sprite::setPaletteColor(uint8_t index, Color color) {
    LGFX_Sprite::setPaletteColor(index, color);
}

void Display::Sprite::fillScreen(Color color) {
    LGFX_Sprite::fillScreen(color);
}

#pragma mark Painter - Pixel

void Display::Sprite::drawPixel(Unit x, Unit y) {
    LGFX_Sprite::drawPixel(x, y);
}

#pragma mark Painter - Line
void Display::Sprite::drawFastVLine(Unit x, Unit y, Unit h) {
    LGFX_Sprite::drawFastVLine(x, y, h);
}

void Display::Sprite::drawFastHLine(Unit x, Unit y, Unit w) {
    LGFX_Sprite::drawFastHLine(x, y, w);
}

#pragma mark Painter - Rect
void Display::Sprite::drawRect(Unit x, Unit y, Unit w, Unit h) {
    LGFX_Sprite::drawRect(x, y, w, h);
}

void Display::Sprite::fillRect(Unit x, Unit y, Unit w, Unit h) {
    LGFX_Sprite::fillRect(x, y, w, h);
}


#pragma mark Painter - String

size_t Display::Sprite::drawString(const char *string, Unit x, Unit y, Align align, const Font *font) {
    switch (align) {
        case Align::Center:
            return LGFX_Sprite::drawCenterString(string, x, y, font);
        case Align::Right:
            return LGFX_Sprite::drawRightString(string, x, y, font);
        case Align::Left:
        default:
            return LGFX_Sprite::drawString(string, x, y, font);
    }
}


#pragma mark Painter - Arc
void Display::Sprite::fillArc(Unit x, Unit y, Unit r0, Unit r1, float angle0, float angle1) {
    LGFX_Sprite::fillArc(x, y, r0, r1, angle0, angle1);
}


#pragma mark Painter - Render
void Display::Sprite::renderSprite(Sprite *sprite, Unit x, Unit y, Color transp) {
    lgfx::pixelcopy_t p(sprite->_img, getColorDepth(), sprite->getColorDepth(), hasPalette(), sprite->_palette, transp);
    pushImage(x, y, sprite->width(), sprite->height(), &p, sprite->_panel_sprite.getSpriteBuffer()->use_dma()); // DMA disable with use SPIRAM
}

void Display::Sprite::renderRotateZoom(Sprite *sprite, float x, float y, float angle, float zoom_x, float zoom_y, Color transp) {
    pushImageRotateZoom(x, y, sprite->pivotX(), sprite->pivotY(), angle, zoom_x, zoom_y, sprite->width(), sprite->height(), sprite->_img, sprite->_write_conv.convert(transp) & sprite->_write_conv.colormask, sprite->getColorDepth(), sprite->_palette.img24());
}

void Display::Sprite::renderRotateZoomAA(Sprite *sprite, float x, float y, float angle, float zoom_x, float zoom_y, Color transp) {
    pushImageRotateZoomWithAA(x, y, sprite->pivotX(), sprite->pivotY(), angle, zoom_x, zoom_y, sprite->width(), sprite->height(), sprite->_img, sprite->_write_conv.convert(transp) & sprite->_write_conv.colormask, sprite->getColorDepth(), sprite->_palette.img24());
}

void Display::Sprite::renderAffine(Sprite *sprite, const float *matrix, Color transp) {
    pushImageAffine(matrix, sprite->width(), sprite->height(), sprite->_img, sprite->_write_conv.convert(transp) & sprite->_write_conv.colormask, sprite->getColorDepth(), sprite->_palette.img24());
}

void Display::Sprite::renderAffineAA(Sprite *sprite, const float *matrix, Color transp) {
    pushImageAffineWithAA(matrix, sprite->width(), sprite->height(), sprite->_img, sprite->_write_conv.convert(transp) & sprite->_write_conv.colormask, sprite->getColorDepth(), sprite->_palette.img24());
}
