from __future__ import annotations
from typing import Optional
from extras import bus
from .crc import crc16_quick
from .error import CRCError


class SPITransaction:
    def __init__(self, spi: bus.MCU_SPI):
        self._spi = spi

    def transfer(self, val) -> bytes:
        params = self._spi.spi_transfer([val])
        return params['response']

    def read_crc(self) -> int:
        return int.from_bytes(self.transfer(0x00) + self.transfer(0x00), "little")

    def read_command(self, cmd: int, length: Optional[int] = None) -> bytes:
        buf = bytearray()

        self.transfer(cmd)
        for i in range(length):
            buf.extend(self.transfer(0x00))
        crc = self.read_crc()

        crc_computed = crc16_quick(buf)
        if crc != crc_computed:
            raise CRCError(crc, crc_computed)

        return bytes(buf)
