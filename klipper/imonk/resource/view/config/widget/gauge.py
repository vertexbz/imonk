from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any
from ..types import Color
from ..field.color import ColorField
from .base import Widget


@dataclass
class GaugeWidget(Widget):
    KIND = "gauge"

    id: str = field(metadata={'example': 'progress', 'description': 'Handle to set gauge value'})
    r_end: int = field(metadata={'alias': 'r-end', 'example': 80, 'description': 'End radius of the gauge'})
    r_start: int = field(metadata={'alias': 'r-start', 'example': 60, 'description': 'Start radius of the gauge'})
    color: Color = ColorField(metadata={'example': 'FF0000', 'description': 'Gauge fill color'})
    background: Color = ColorField(default="000000", metadata={'description': 'Gauge background color'})
    value: int = field(default=0, metadata={'description': 'Gauge value, i.e. how much of the gauge should be filled'})
    rotation: float = field(default=90.0, metadata={'description': 'Gauge widget rotation (in degrees)'})
    arc: float = field(default=260.0, metadata={'description': 'How much of the circle should be used as gauge (in degrees)'})

    def get_slots(self) -> list[tuple[str, type, Any]]:
        return [(self.id, int, self.value)]
