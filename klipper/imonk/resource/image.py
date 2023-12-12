from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from configfile import ConfigWrapper
    from klippy import Printer
    from ..manager import IMONKManager


class IMONKResourceImage:
    def __init__(self, config: ConfigWrapper, name: str, manager: IMONKManager):
        self.manager = manager
        self.name = name
        self.printer: Printer = config.get_printer()
