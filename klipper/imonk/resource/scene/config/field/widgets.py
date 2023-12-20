from __future__ import annotations
from typing import Union
from ..widget import build_widget
from ..widget.base import Widget


class WidgetsField:
    class DEFAULT: ...

    def __init__(self, *, default=DEFAULT, default_factory=list):
        self._default_factory = default_factory
        self._default = default

    def __set_name__(self, owner, name):
        self._name = "_" + name

    def __get__(self, obj, owner):
        if obj is None:
            return self._default

        return getattr(obj, self._name, self._default)

    def __set__(self, obj, value: Union[list[dict], list[Widget]]):
        if value == self.DEFAULT:
            value = self._default_factory()

        if not isinstance(value, list):
            raise ValueError('List expected')

        if all(map(lambda w: isinstance(w, Widget), value)):
            return setattr(obj, self._name, value)

        return setattr(obj, self._name, list(map(lambda w: self.build_widget(w), value)))

    def build_widget(self, data: dict) -> Widget:
        if not isinstance(data, dict):
            raise ValueError('Object expected')

        kind = data.pop('type')
        return build_widget(kind, {k.replace('-', '_'): v for k, v in data.items()})
