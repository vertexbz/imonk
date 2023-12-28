from __future__ import annotations
from dataclasses import dataclass
from typing import Any
from ..base import Base


@dataclass
class Widget(Base):
    KIND = ""

    x: int
    y: int

    def _fields(self):
        yield 'type', type(self).KIND
        yield from super()._fields()

    def get_slots(self) -> list[tuple[str, type, Any]]:
        return []
