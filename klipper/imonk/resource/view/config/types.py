from __future__ import annotations
from re import match as rematch
from ....generated.font import KNOWN_FONTS


class Color(str):
    def __new__(cls, value: str):
        if not isinstance(value, str) or len(value) not in (6, 8):
            raise ValueError('Hex color expected')

        value = value.upper()
        if not rematch(r'^[0-9A-F]{6}([0-9A-F]{2})?$', value):
            raise ValueError('Hex color expected')

        return super().__new__(cls, value)


class Align(str):
    def __new__(cls, value: str):
        align = value.lower()
        if align not in ('center', 'left', 'right'):
            raise ValueError(f'Invalid alignment {value}')

        return super().__new__(cls, align)


class Font(str):
    def __new__(cls, value: str):
        font = value.lower()
        if font not in KNOWN_FONTS:
            raise ValueError(f'Unknown font {value}')

        return super().__new__(cls, font)
