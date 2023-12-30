from __future__ import annotations
from dataclasses import fields


class Base:
    @classmethod
    def sanitize(cls, data: dict):
        field_map = cls.__data_field_map_in__()
        return {field_map.get(k, k): v for k, v in data.items()}

    def dump(self):
        field_map = self.__data_field_map_out__()
        return dict([(field_map.get(k, k), self._field_value(v)) for k, v in self._fields()])

    @classmethod
    def __data_field_map_out__(cls) -> dict[str, str]:
        return {k: f.metadata['alias'] for k, f in cls.__dataclass_fields__.items() if 'alias' in f.metadata}

    @classmethod
    def __data_field_map_in__(cls) -> dict[str, str]:
        return {f.metadata['alias']: k for k, f in cls.__dataclass_fields__.items() if 'alias' in f.metadata}

    def _fields(self):
        for f in fields(self):
            yield f.name, getattr(self, f.name)

    @classmethod
    def _field_value(cls, value):
        if isinstance(value, (list, tuple, set)):
            value = type(value)(map(lambda v: cls._field_value(v), value))
        if isinstance(value, Base):
            value = value.dump()
        return value
