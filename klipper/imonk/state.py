from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from .resource import IMONKResourceImage
    from .resource import IMONKResourceScene


class DeviceState:
    def __init__(self):
        self.images: dict[str, int] = {}
        self.scenes: dict[str, int] = {}


class HostState:
    def __init__(self):
        self.images: dict[str, IMONKResourceImage] = {}
        self.scenes: dict[str, IMONKResourceScene] = {}


class State:
    def __init__(self):
        self.device = DeviceState()
        self.host = HostState()
