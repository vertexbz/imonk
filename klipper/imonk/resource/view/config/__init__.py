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
    background: str = ColorField()
    widgets: list[Widget] = WidgetsField()
