from __future__ import annotations
from typing import Optional
from .base import BaseField
from ..types import Font


class FontField(BaseField):
    def __init__(self, *, default=BaseField.DEFAULT, metadata: Optional[dict] = None):
        super().__init__(Font, default=default, metadata=metadata)

    def _normalize(self, value):
        return Font(value)
