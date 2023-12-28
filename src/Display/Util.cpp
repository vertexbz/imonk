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
            _unicode_buffer |= (c & 0x3F)<<6;
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

Display::Color Display::Util::strtorgb(const char *str) {
    Color c = 0;
    for (uint8_t i = 0; i < 6 && *str > 0; ++i, ++str) {
        c <<= 4;
        if (*str >= '0' && *str <= '9') {
            c |= *str - '0';
        } else if (*str >= 'A' && *str <= 'F') {
            c |= 10 + *str - 'A';
        } else if (*str >= 'a' && *str <= 'f') {
            c |= 10 + *str - 'a';
        }
    }
    return c;
}

Display::Align Display::Util::strtoalign(const char *str) {
    if (strcmp(str, "left") == 0) {
        return Align::Left;
    }
    if (strcmp(str, "right") == 0) {
        return Align::Right;
    }
    return Align::Center;
}

#define fnt(f) const_cast<std::remove_const_t<decltype(f)>*>(&f);
#define fnt_if(name, f) if (strcmp(str, name) == 0) return fnt(f);
Display::Font * Display::Util::strtofont(const char *str) {
    fnt_if("font0", Font0);
    fnt_if("font2", Font2);
    fnt_if("font4", Font4);
    fnt_if("font6", Font6);
    fnt_if("font7", Font7);
    fnt_if("font8", Font8);
    fnt_if("font8c64", Font8x8C64);
    fnt_if("ascii8", AsciiFont8x16);
    fnt_if("ascii24", AsciiFont24x48);
    fnt_if("orbitron24", Orbitron_Light_24);
    fnt_if("orbitron32", Orbitron_Light_32);
    fnt_if("roboto24", Roboto_Thin_24);
    return fnt(Font0);
}
