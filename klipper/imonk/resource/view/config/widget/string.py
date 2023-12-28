from __future__ import annotations
from dataclasses import dataclass
from typing import Any
from ..field.color import ColorField
from .base import Widget


@dataclass
class StringWidget(Widget):
    KIND = "string"

    id: str
    color: str = ColorField()
    value: str = ""

    def get_slots(self) -> list[tuple[str, type, Any]]:
        return [(self.id, str, self.value)]
