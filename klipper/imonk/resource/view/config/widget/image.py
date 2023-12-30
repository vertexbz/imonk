from __future__ import annotations
from dataclasses import dataclass, field
from .base import Widget


@dataclass
class ImageWidget(Widget):
    KIND = "image"

    name: str = field(metadata={'example': 'smily-face', 'description': 'Name of the image to display - same as in [imonk image ...] section'})
