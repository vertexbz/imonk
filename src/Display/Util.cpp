//
// Created by Adam Makswiej on 21/11/2023.
//

#include "Util.hpp"

wchar Display::Util::UTF8Decoder::decode(const char c) {
    // 7 bit Unicode Code Point
    if (!(c & 0x80)) {
        _decoderState = 0;
        return c;
    }

    if (_decoderState == 0)
    {
        // 11 bit Unicode Code Point
        if ((c & 0xE0) == 0xC0)
        {
            _unicode_buffer = ((c & 0x1F)<<6);
            _decoderState = 1;
            return 0;
        }

        // 16 bit Unicode Code Point
        if ((c & 0xF0) == 0xE0)
        {
            _unicode_buffer = ((c & 0x0F)<<12);
            _decoderState = 2;
            return 0;
        }
        // 21 bit Unicode  Code Point not supported so fall-back to extended ASCII
        //if ((c & 0xF8) == 0xF0) return (uint16_t)c;
    }
    else
    {
        if (_decoderState == 2)
        {
            _unicode_buffer |= ((c & 0x3F)<<6);
            _decoderState = 1;
            return 0;
        }
        _unicode_buffer |= (c & 0x3F);
        _decoderState = 0;
        return _unicode_buffer;
    }

    _decoderState = 0;

    return c; // fall-back to extended ASCII

}

Display::Unit Display::Util::fontHeight(const Display::Font *font) {
    lgfx::FontMetrics metrics{0};
    font->getDefaultMetric(&metrics);
    return metrics.height;
}

Display::Unit Display::Util::textWidth(const char *string, const Display::Font *font) {
    lgfx::FontMetrics metrics{0};
    font->getDefaultMetric(&metrics);

    if (!string || !string[0]) return 0;

    int32_t sx = 65536 * 1.0; // _text_style.size_x = 1.0; // scaling

    int32_t left = 0;
    int32_t right = 0;

    UTF8Decoder decoder{};
    do {
        uint16_t uniCode = *string;
        if (true) { // _text_style.utf8 unicode support
            do {
                uniCode = decoder.decode(*string);
            } while (uniCode < 0x20 && *(++string));
            if (uniCode < 0x20) break;
        }

        //if (!_font->updateFontMetric(&metrics, uniCode)) continue;
        font->updateFontMetric(&metrics, uniCode);

        int32_t sxoffset = (metrics.x_offset * sx) >> 16;
        if (left == 0 && right == 0 && metrics.x_offset < 0) left = right = - sxoffset;
        int32_t sxadvance = (metrics.x_advance * sx) >> 16;
        right = left + std::max<int>(sxadvance, ((metrics.width * sx) >> 16) + sxoffset);
        //right = left + (int)(std::max<int>(metrics->x_advance, metrics->width + metrics->x_offset) * sx);
        left += sxadvance;
    } while (*(++string));
    return right;
}
