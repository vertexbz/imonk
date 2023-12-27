from __future__ import annotations
from typing import TYPE_CHECKING
from .gauge import GaugeWidget
from .image import ImageWidget
from .string import StringWidget

if TYPE_CHECKING:
    from .base import Widget


WIDGETS = [GaugeWidget, ImageWidget, StringWidget]


def build_widget(kind: str, data: dict) -> Widget:
    for WidgetClass in WIDGETS:
        if WidgetClass.KIND == kind:
            return WidgetClass(**data)

    raise ValueError(f'Unknown widget type {kind}')


__all__ = ['build_widget']
