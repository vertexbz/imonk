from __future__ import annotations
from typing import Optional
from .base import BaseField
from ..types import Align


class AlignField(BaseField):
    def __init__(self, *, default=BaseField.DEFAULT, metadata: Optional[dict] = None):
        super().__init__(Align, default=default, metadata=metadata)

    def _normalize(self, value):
        return Align(value)
