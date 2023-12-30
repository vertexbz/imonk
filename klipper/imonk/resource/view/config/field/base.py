from __future__ import annotations
from typing import Optional


class BaseField:
    class DEFAULT: ...

    def __init__(self, typ, *, default=DEFAULT, default_factory=DEFAULT, metadata: Optional[dict] = None):
        self.type = typ
        self.default = default
        self.default_factory = default_factory
        self.metadata = metadata

    def __set_name__(self, _, name):
        self._name = "_" + name

    def __get__(self, obj, _):
        if obj is None:
            return self.default

        return getattr(obj, self._name, self.default)

    def __set__(self, obj, value):
        if value == self.DEFAULT:
            value = self.default_factory()

        return setattr(obj, self._name, self._normalize(value))

    def _normalize(self, value):
        return value
