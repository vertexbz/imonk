from __future__ import annotations
from typing import TYPE_CHECKING
from dataclasses import dataclass
from .field.color import ColorField
from .field.widgets import WidgetsField
from .base import Base

if TYPE_CHECKING:
    from .widget.base import Widget


@dataclass
class View(Base):
    background: str = ColorField(metadata={'example': '333333', 'description': 'View background color'})
    widgets: list[Widget] = WidgetsField(metadata={'description': 'List of widgets to render on the view'})
