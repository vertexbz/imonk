from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any
from ..types import Color, Font, Align
from ..field.color import ColorField
from ..field.align import AlignField
from ..field.font import FontField
from .base import Widget


@dataclass
class StringWidget(Widget):
    KIND = "string"

    id: str = field(metadata={'example': 'text1', 'description': 'Handle to set widgets text'})
    color: Color = ColorField(metadata={'example': 'FFFFFF', 'description': 'Text color'})
    value: str = field(default="", metadata={'example': 'Testy text', 'description': 'Text to display on screen'})
    font: Font = FontField(default="font0", metadata={'example': 'roboto24', 'description': 'Font to use to render the text'})
    align: Align = AlignField(default="center", metadata={'example': 'left', 'description': 'Text alignment in relation to provided coordinates'})

    def get_slots(self) -> list[tuple[str, type, Any]]:
        return [(self.id, str, self.value)]
