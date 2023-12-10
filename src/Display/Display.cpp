//
// Created by Adam Makswiej on 20/11/2023.
//

#include "Display.hpp"
#include "Sprite.hpp"
#include "Scene.hpp"

Display::Display::Display(): LGFX_GC9A01(), Painter(), _canvas(width(), height()) {
}

bool Display::Display::begin() {
    if (LGFX_GC9A01::begin(PIN_LCD_SCLK, PIN_LCD_MOSI, PIN_LCD_DC, PIN_LCD_CS, PIN_LCD_RST, PIN_LCD_BL)) {
        setBrightness(5);
//        _canvas.init();
        return true;
    }
    return false;
}

void Display::Display::render(Scene *scene) {
//    LGFX_GC9A01::beginTransaction();
    LGFX_GC9A01::startWrite();

//    _canvas.fillScreen(scene->background());
//    LGFX_GC9A01::clearDisplay(scene->background());
//    LGFX_GC9A01::setColor(scene->background());
//    LGFX_GC9A01::writeFillRectPreclipped(0,0,width(), height());
//    scene->render(&_canvas);
    scene->render(this);

//    LGFX_GC9A01::startWrite();
//    renderSprite(&_canvas, 0, 0);
    LGFX_GC9A01::endWrite();
//    LGFX_GC9A01::display();
//    LGFX_GC9A01::endTransaction();
}


#pragma mark Painter - Info
Display::Unit Display::Display::width() const {
    return 240;
}

Display::Unit Display::Display::height() const {
    return 240;
}

float Display::Display::pivotX() const {
    return _xpivot;
}

float Display::Display::pivotY() const {
    return _ypivot;
}

#pragma mark Painter - Misc
void Display::Display::setPivot(float x, float y) {
    LGFX_GC9A01::setPivot(x, y);
}

void Display::Display::setColor(Color color) {
    if (color == Background) {
        color = _text_style.back_rgb888;
    }
    LGFX_GC9A01::setColor(color);
}

void Display::Display::setTextColor(Color color) {
    if (color == Background) {
        color = _text_style.back_rgb888;
    }
    LGFX_GC9A01::setTextColor(color, _text_style.back_rgb888);
}

void Display::Display::setPaletteColor(uint8_t index, Color color) {

}

void Display::Display::setColorDepth(ColorDepth depth) {
    LGFX_GC9A01::setColorDepth((uint16_t)depth);
}

void Display::Display::fillScreen(Color color) {
    LGFX_GC9A01::fillScreen(color);
}

void Display::Display::clearScreen(Color color) {
    LGFX_GC9A01::setBaseColor(color);
    LGFX_GC9A01::setTextColor(_text_style.fore_rgb888, color);
    LGFX_GC9A01::clearDisplay(color);
}

#pragma mark Painter - Pixel

void Display::Display::drawPixel(Unit x, Unit y) {
    LGFX_GC9A01::drawPixel(x, y);
}

#pragma mark Painter - Line
void Display::Display::drawFastVLine(Unit x, Unit y, Unit h) {
    LGFX_GC9A01::drawFastVLine(x, y, h);
}

void Display::Display::drawFastHLine(Unit x, Unit y, Unit w) {
    LGFX_GC9A01::drawFastHLine(x, y, w);
}

#pragma mark Painter - Rect
void Display::Display::drawRect(Unit x, Unit y, Unit w, Unit h) {
    LGFX_GC9A01::drawRect(x, y, w, h);
}

void Display::Display::fillRect(Unit x, Unit y, Unit w, Unit h) {
    LGFX_GC9A01::fillRect(x, y, w, h);
}

#pragma mark Painter - Arc
void Display::Display::fillArc(Unit x, Unit y, Unit r0, Unit r1, float angle0, float angle1) {
    LGFX_GC9A01::fillArc(x, y, r0, r1, angle0, angle1);
}

#pragma mark Painter - String
size_t Display::Display::drawString(const char *string, Unit x, Unit y, Align align, const Font *font) {
    switch (align) {
        case Align::Center:
            return LGFX_GC9A01::drawCenterString(string, x, y, font);
        case Align::Right:
            return LGFX_GC9A01::drawRightString(string, x, y, font);
        case Align::Left:
        default:
            return LGFX_GC9A01::drawString(string, x, y, font);
    }
}

#pragma mark Painter - Images
bool Display::Display::drawPng(File *file, Unit x, Unit y, Unit maxWidth, Unit maxHeight, Unit offX, Unit offY, float scaleX, float scaleY, Align2D align) {
    if (!file) return false;
    return this->draw_png(file, x, y, maxWidth, maxHeight, offX, offY, scaleX, scaleY, static_cast<datum_t>(align));
}

#pragma mark Painter - Render
void Display::Display::renderSprite(Sprite *sprite, Unit x, Unit y, Color transp) {
    lgfx::pixelcopy_t p(sprite->_img, getColorDepth(), sprite->getColorDepth(), hasPalette(), sprite->_palette, transp);
    pushImage(x, y, sprite->width(), sprite->height(), &p, sprite->_panel_sprite.getSpriteBuffer()->use_dma()); // DMA disable with use SPIRAM
}

void Display::Display::renderRotateZoom(Sprite *sprite, float x, float y, float angle, float zoom_x, float zoom_y, Color transp) {
    pushImageRotateZoom(x, y, sprite->pivotX(), sprite->pivotY(), angle, zoom_x, zoom_y, sprite->width(), sprite->height(), sprite->_img, sprite->_write_conv.convert(transp) & sprite->_write_conv.colormask, sprite->getColorDepth(), sprite->_palette.img24());
}

void Display::Display::renderRotateZoomAA(Sprite *sprite, float x, float y, float angle, float zoom_x, float zoom_y, Color transp) {
    pushImageRotateZoomWithAA(x, y, sprite->pivotX(), sprite->pivotY(), angle, zoom_x, zoom_y, sprite->width(), sprite->height(), sprite->_img, sprite->_write_conv.convert(transp) & sprite->_write_conv.colormask, sprite->getColorDepth(), sprite->_palette.img24());
}

void Display::Display::renderAffine(Sprite *sprite, const float *matrix, Color transp) {
    pushImageAffine(matrix, sprite->width(), sprite->height(), sprite->_img, sprite->_write_conv.convert(transp) & sprite->_write_conv.colormask, sprite->getColorDepth(), sprite->_palette.img24());
}

void Display::Display::renderAffineAA(Sprite *sprite, const float *matrix, Color transp) {
    pushImageAffineWithAA(matrix, sprite->width(), sprite->height(), sprite->_img, sprite->_write_conv.convert(transp) & sprite->_write_conv.colormask, sprite->getColorDepth(), sprite->_palette.img24());
}

