from __future__ import annotations
from dataclasses import dataclass
from ..base import Base


@dataclass
class Widget(Base):
    KIND = ""

    x: int = 0
    y: int = 0

    def _fields(self):
        yield 'type', type(self).KIND
        yield from super()._fields()
