from __future__ import annotations
import os.path
from re import match as rematch


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
    @classmethod
    def __get_fonts__(cls):
        key = '__known_fonts__'
        if not hasattr(cls, key):
            result = []
            with open(os.path.join(os.path.dirname(__file__), '../../../../../src/Display/Util.cpp'), 'r') as f:
                for line in f.readlines():
                    line = line.strip()
                    if line.startswith('fnt_if'):
                        result.append(line.split('"')[1])
            result = tuple(result)
            setattr(cls, key, result)
            return result
        return getattr(cls, key)

    def __new__(cls, value: str):
        font = value.lower()
        if font not in cls.__get_fonts__():
            raise ValueError(f'Unknown font {value}')

        return super().__new__(cls, font)
