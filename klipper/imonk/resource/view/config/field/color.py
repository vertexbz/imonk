from __future__ import annotations
import re


class ColorField:
    def __init__(self, *, default: str = "000000"):
        self._default = default

    def __set_name__(self, owner, name):
        self._name = "_" + name

    def __get__(self, obj, owner):
        if obj is None:
            return self._default

        return getattr(obj, self._name, self._default)

    def __set__(self, obj, value: str):
        if not isinstance(value, str) or len(value) not in (6, 8):
            raise ValueError('Hex color expected')

        value = value.upper()
        if not re.match(r'^[0-9A-F]{6}([0-9A-F]{2})?$', value):
            raise ValueError('Hex color expected')

        return setattr(obj, self._name, value)
