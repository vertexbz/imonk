from __future__ import annotations

from typing import Optional

from .base import BaseField
from ..widget import build_widget
from ..widget.base import Widget


class WidgetsField(BaseField):
    def __init__(self, *, metadata: Optional[dict] = None):
        super().__init__(list, default_factory=list, metadata=metadata)

    def _normalize(self, value):
        if not isinstance(value, list):
            raise ValueError('List expected')

        if all(map(lambda w: isinstance(w, Widget), value)):
            return value

        return list(map(self.build_widget, value))

    def build_widget(self, data: dict) -> Widget:
        if not isinstance(data, dict):
            raise ValueError('Object expected')

        kind = data.pop('type')
        return build_widget(kind, data)
