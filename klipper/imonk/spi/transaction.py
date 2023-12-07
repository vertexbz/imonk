from __future__ import annotations
from typing import Optional
from time import sleep
from extras import bus
from .crc import crc16_quick, crc16_step
from .error import CRCError, CommunicationError


class SPITransaction:
    def __init__(self, spi: bus.MCU_SPI, gcode):  # TODO
        self._spi = spi
        self._gcode = gcode

    def transfer(self, byte: int) -> int:
        params = self._spi.spi_transfer([byte])
        response = params['response'][0]
        self._gcode.respond_info(f'out: {hex(byte)[2:].upper()}, in: {hex(response)[2:].upper()}')
        return response

    def read_word(self) -> int:
        return int.from_bytes([self.transfer(0x00), self.transfer(0x00)], 'little')

    def read_command(self, cmd: int, length: Optional[int] = None) -> bytes:
        buf = bytearray()
        crc_buf = bytearray()

        self.transfer(cmd)
        if length is None:
            length = self.read_word()
            crc_buf.extend(length.to_bytes(2, 'little'))

        for i in range(length):
            buf.append(self.transfer(0x00))

        # CRC
        crc = self.read_word()
        crc_buf.extend(buf)
        crc_computed = crc16_quick(crc_buf)
        if crc != crc_computed:
            raise CRCError(crc, crc_computed)

        return bytes(buf)

    def duplex_write(self, val: int, last: int) -> None:
        response = self.transfer(val)
        if response != last:
            raise CommunicationError(f'Expected echo of last byte {hex(last)[2:].upper()}, but received {hex(response)[2:].upper()}')

    def zero_write(self, val: int) -> None:
        response = self.transfer(val)
        if response != 0:
            raise CommunicationError(f'Expected 00, but received {hex(response)[2:].upper()}')

    def write(self, data: bytes, var_len: bool = False, crc: bool = False, delay_epilogue: float = 0.) -> None:
        last = 0
        crc_computed = 0xFFFF

        self._gcode.respond_info(f'out: {data.hex().upper()} var: {var_len} crc {crc}')

        if var_len:
            for b in len(data).to_bytes(2, 'little'):
                if crc:
                    crc_computed = crc16_step(b, crc_computed)
                    self.zero_write(b)
                else:
                    self.duplex_write(b, last)
                    last = b

        for b in data:
            if crc:
                crc_computed = crc16_step(b, crc_computed)
                self.zero_write(b)
            else:
                self.duplex_write(b, last)
                last = b

        if crc:
            for b in crc_computed.to_bytes(2, 'little', signed=False):
                self.zero_write(b)
        else:
            self.duplex_write(0x00, last)

        sleep(delay_epilogue)
        self.epilogue(crc_computed)

    def write_segmented(self, data: bytes, crc: bool = False, segment_size: int = 16) -> None:
        last = 0
        crc_computed = 0xFFFF
        wait = 0.2

        for bb in [data[i:i+segment_size] for i in range(0, len(data), segment_size)]:
            for b in bb:
                crc_computed = crc16_step(b, crc_computed)
                self.duplex_write(b, last)
                sleep(wait)
                last = b

            self.duplex_write(0, last)
            sleep(wait)
            last = 0

            self.duplex_write(0, last)  # TODO repeat when duplex error CC
            sleep(wait)
            last = 0

        if crc:
            for b in crc_computed.to_bytes(2, 'little', signed=False):
                self.zero_write(b)
                sleep(wait)
        else:
            self.duplex_write(0x00, last)
            sleep(wait)

        self.epilogue(crc_computed)

    def write_command(self, cmd: int, data: bytes, var_len: bool = False, crc: bool = False) -> None:
        self.transfer(cmd)
        self.write(data, var_len, crc)

    def upload_command(self, cmd: int, name: str, data: bytes, crc: bool = False):
        self.transfer(cmd)
        self.write(name.encode('ascii'), True, False)
        self.write(len(data).to_bytes(4, 'little', signed=False), False, False, delay_epilogue=0.2)
        sleep(1)
        self.write_segmented(data, crc)

    def epilogue(self, crc_computed: int):
        result = self.transfer(0x00)
        if result == 0x01:
            bcrc = crc_computed.to_bytes(2, 'little', signed=False)
            raise CommunicationError(f'CRC failed {bcrc.hex().upper()}')
        if result == 0x02:
            raise CommunicationError('Invalid value')
        if result not in (0xA5, 0xA4):
            raise CommunicationError(f'Write failed {hex(result)[2:].upper()}')
