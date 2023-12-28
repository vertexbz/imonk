from __future__ import annotations
from dataclasses import dataclass
from typing import Any

from ..field.color import ColorField
from .base import Widget


@dataclass
class GaugeWidget(Widget):
    KIND = "gauge"

    id: str
    r_end: int
    r_start: int
    color: str = ColorField()
    background: str = ColorField()
    value: int = 0

    def _field_name(self, name: str) -> str:
        return name.replace('r_', 'r-')

    def get_slots(self) -> list[tuple[str, type, Any]]:
        return [(self.id, int, self.value)]
