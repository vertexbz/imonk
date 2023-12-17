from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from configfile import ConfigWrapper
    from klippy import Printer
    from ..manager import IMONKManager


class IMONKResourceScene:
    def __init__(self, config: ConfigWrapper, name: str, manager: IMONKManager):
        self.name = name
        self.printer: Printer = config.get_printer()

        manager.state.host.scenes[self.name] = self

    @property
    def crc(self) -> int:
        return 0

    @property
    def data(self) -> bytes:
        return b''
