from __future__ import annotations
from typing import Optional
from extras import bus
from .crc import crc16_quick
from .error import CRCError, CommunicationError


class SPITransaction:
    def __init__(self, spi: bus.MCU_SPI):
        self._spi = spi

    def transfer(self, val) -> bytes:
        params = self._spi.spi_transfer([val])
        return params['response']

    def read_word(self) -> int:
        return int.from_bytes(self.transfer(0x00) + self.transfer(0x00), 'little')

    def read_command(self, cmd: int, length: Optional[int] = None) -> bytes:
        buf = bytearray()
        crc_buf = bytearray()

        self.transfer(cmd)
        if length is None:
            length = self.read_word()
            crc_buf.extend(length.to_bytes(2, 'little'))

        for i in range(length):
            buf.extend(self.transfer(0x00))

        # CRC
        crc = self.read_word()
        crc_buf.extend(buf)
        crc_computed = crc16_quick(crc_buf)
        if crc != crc_computed:
            raise CRCError(crc, crc_computed)

        return bytes(buf)

    def duplex_write(self, val: int, last: int) -> None:
        response = self.transfer(val)[0]
        if response != last:
            raise CommunicationError(f'Expected echo of last byte {hex(last)[2:].upper()}, but received {hex(response)[2:].upper()}')

    def write_command(self, cmd: int, data: bytes, crc: bool = False) -> None:  # TODO CRC
        self.transfer(cmd)

        last = 0
        for b in len(data).to_bytes(2, 'little'):
            self.duplex_write(b, last)
            last = b

        for b in data:
            self.duplex_write(b, last)
            last = b

        self.duplex_write(0x00, last)

        result = self.transfer(0x00)[0]
        if result != 0xA5:
            raise CommunicationError('Write failed')

