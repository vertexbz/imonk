from __future__ import annotations
from typing import TYPE_CHECKING
from os.path import expanduser
from ..spi.crc import crc16_step

if TYPE_CHECKING:
    from configfile import ConfigWrapper
    from klippy import Printer
    from ..manager import IMONKManager


class IMONKResourceImage:
    def __init__(self, config: ConfigWrapper, name: str, manager: IMONKManager):
        self.name = name
        self.path = config.get('path')
        self.printer: Printer = config.get_printer()

        manager.state.host.images[self.name] = self

    @property
    def crc(self) -> int:
        with open(expanduser(self.path), 'rb') as f:
            crc = 0xFFFF
            while True:
                bytes_in = f.read(1)
                if not bytes_in:
                    break
                byte = bytes_in[0]
                crc = crc16_step(byte, crc)

            return crc & 0xFFFF

    @property
    def data(self) -> bytes:
        with open(expanduser(self.path), 'rb') as f:
            return f.read()
