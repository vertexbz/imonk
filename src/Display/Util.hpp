#pragma once

#include "types.hpp"

namespace Display { namespace Util {
        class UTF8Decoder {
        protected:
            uint8_t _decoderState = 0;
            uint_fast16_t _unicode_buffer = 0;

        public:
            wchar decode(char c);
        };

        Unit fontHeight(const Font *font);

        Unit textWidth(const char *string, const Font *font);

        inline Unit textWidth(const String &string, const Font *font) { return textWidth(string.c_str(), font); }

        Color strtorgb(const char *str);

        Align strtoalign(const char *str);

        Font *strtofont(const char *str);
    };
}
