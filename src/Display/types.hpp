#pragma once
#include <stddef.h>
#include <stdint.h>
#include <lgfx/v1/lgfx_fonts.hpp>

typedef uint16_t  wchar;

namespace Display {
    typedef uint32_t Color;
    const Color NoTransparent = 1 << 24;
    const Color Background = 1 << 25;

    typedef int32_t Unit;
    typedef lgfx::IFont Font;
    namespace Fonts = fonts;

    enum class Align : uint8_t {
        Left = 0,
        Center = 1,
        Right = 2,
    };

    enum class ColorDepth : uint16_t
    {
        bit_mask     = 0x00FF   , /// ビット数取得用マスク値 (下位1Byteはビット数表現専用とする。変更しないこと);
        has_palette  = 0x0800   , /// パレット値として扱う;
        nonswapped   = 0x0100   , /// バイトスワップしていない値;
        alternate    = 0x1000   , /// ビット数が同一な色表現が複数ある場合の相違表現用;

        grayscale_1bit      =   1                         , //                            _______L
        grayscale_2bit      =   2                         , //                            ______LL
        grayscale_4bit      =   4                         , //                            ____LLLL
        grayscale_8bit      =   8 |              alternate, // ________ ________ ________ LLLLLLLL
        palette_1bit        =   1 | has_palette           , //                            _______I   2 color
        palette_2bit        =   2 | has_palette           , //                            ______II   4 color
        palette_4bit        =   4 | has_palette           , //                            ____IIII  16 color
        palette_8bit        =   8 | has_palette           , //                            IIIIIIII 256 color
        rgb332_1Byte        =   8                         , // ________ ________ ________ RRRGGGBB
        rgb565_2Byte        =  16                         , // ________ ________ GGGBBBBB RRRRRGGG
        rgb666_3Byte        =  24 |              alternate, // ________ __BBBBBB __GGGGGG __RRRRRR
        rgb888_3Byte        =  24                         , // ________ BBBBBBBB GGGGGGGG RRRRRRRR
        argb8888_4Byte      =  32                         , // BBBBBBBB GGGGGGGG RRRRRRRR AAAAAAAA
        rgb565_nonswapped   =  16 | nonswapped            , // ________ ________ RRRRRGGG GGGBBBBB
        rgb666_nonswapped   =  24 | nonswapped | alternate, // ________ __RRRRRR __GGGGGG __BBBBBB
        rgb888_nonswapped   =  24 | nonswapped            , // ________ RRRRRRRR GGGGGGGG BBBBBBBB
        argb8888_nonswapped =  32 | nonswapped            , // AAAAAAAA RRRRRRRR GGGGGGGG BBBBBBBB
    };
}
