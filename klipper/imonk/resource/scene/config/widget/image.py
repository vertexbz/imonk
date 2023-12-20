from __future__ import annotations
from dataclasses import dataclass
from .base import Widget


@dataclass
class ImageWidget(Widget):
    KIND = "image"

    name: str = ""
