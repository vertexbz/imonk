from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any
from ..base import Base


@dataclass
class Widget(Base):
    KIND = ""

    x: int = field(metadata={'example': 120, 'description': 'X Coordinate of the widget'})
    y: int = field(metadata={'example': 120, 'description': 'Y Coordinate of the widget'})

    def _fields(self):
        yield 'type', type(self).KIND
        yield from super()._fields()

    def get_slots(self) -> list[tuple[str, type, Any]]:
        return []
