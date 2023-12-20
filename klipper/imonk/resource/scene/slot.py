from __future__ import annotations


class Slot:
    def __init__(self, id: int, typ: type):
        self._id = id
        self._type = typ
        self._type_id = Slot.type_to_id(typ)

    def get_id(self) -> int:
        return self._id

    def get_type_id(self) -> int:
        return self._type_id

    def get_type(self) -> type:
        return self._type

    @staticmethod
    def type_to_id(typ: type) -> int:
        if isinstance(typ, str):
            return 0x57
        if isinstance(typ, bool):
            return 0x01
        if isinstance(typ, int):
            return 0x69
        if isinstance(typ, float):
            return 0x42

        raise ValueError(f'Unsupported type {typ}')
