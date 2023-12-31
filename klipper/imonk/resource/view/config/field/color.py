from __future__ import annotations
from typing import Optional
from .base import BaseField
from ..types import Color


class ColorField(BaseField):
    def __init__(self, *, default=BaseField.DEFAULT, metadata: Optional[dict] = None):
        super().__init__(Color, default=default, metadata=metadata)

    def _normalize(self, value):
        return Color(value)
