from __future__ import annotations
from dataclasses import fields


class Base:
    def dump(self):
        result = []
        for k, v in self._fields():
            result.append((self._field_name(k), self._field_value(v)))
        return dict(result)

    def _fields(self):
        for f in fields(self):
            yield f.name, getattr(self, f.name)

    def _field_name(self, name: str) -> str:
        return name

    def _field_value(self, value):
        if isinstance(value, (list, tuple, set)):
            value = type(value)(map(lambda v: self._field_value(v), value))
        if isinstance(value, Base):
            value = value.dump()
        return value
