from __future__ import annotations
from dataclasses import dataclass
from ..field.color import ColorField
from .base import Widget


@dataclass
class GaugeWidget(Widget):
    KIND = "gauge"

    r_end: int = 0
    r_start: int = 0
    color: str = ColorField()
    background: str = ColorField()

    def _field_name(self, name: str) -> str:
        return name.replace('r_', 'r-')
