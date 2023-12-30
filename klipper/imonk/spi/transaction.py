from __future__ import annotations
from functools import cached_property
from typing import Optional, Union
from typing import TYPE_CHECKING
from time import sleep
from .crc import crc16_quick, crc16_step
from .error import CRCError, CommunicationError, UnexpectedResponse, ByteCounter, StopReading
from .const import CRCOnlyLength

if TYPE_CHECKING:
    from gcode import GCodeDispatch
    from ..klipper import MCU_SPI


class SPITransaction:
    def __init__(self, spi: MCU_SPI):
        self._spi = spi

    @cached_property
    def gcode(self) -> GCodeDispatch:
        return self._spi.mcu.get_printer().lookup_object('gcode')

    def transfer(self, byte: int) -> int:
        params = self._spi.spi_transfer([byte])
        response = params['response'][0]
        return response

    def read_word(self) -> int:
        return int.from_bytes([self.transfer(0x00), self.transfer(0x00)], 'little')

    def duplex_write(self, byte: int, expected: int) -> int:
        response = self.transfer(byte)
        if response != expected:
            raise UnexpectedResponse(expected, response)
        return byte

    def epilogue(self, crc_computed: int):
        result = self.transfer(0x00)
        if result == 0x01:
            raise CRCError(crc_computed)
        if result == 0x02:
            raise CommunicationError('Invalid value provided.')
        if result not in (0xA5, 0xA4):
            raise CommunicationError(f'Write failed, response: {hex(result)[2:].upper().zfill(2)}.')

    def write(self, data: bytes, var_len: Union[bool, CRCOnlyLength] = False, crc: bool = False) -> int:
        last = 0
        crc_computed = 0xFFFF

        with ByteCounter('data') as counter:
            if var_len:
                with counter.sub('length') as l_counter:
                    for b in len(data).to_bytes(2, 'little'):
                        if crc or var_len == CRCOnlyLength:
                            crc_computed = crc16_step(b, crc_computed)
                            if var_len != CRCOnlyLength:
                                self.duplex_write(b, 0)
                        else:
                            last = self.duplex_write(b, last)
                        l_counter.inc()

            with counter.sub('bytes') as b_counter:
                for b in data:
                    if crc:
                        crc_computed = crc16_step(b, crc_computed)
                        self.duplex_write(b, 0)
                    else:
                        last = self.duplex_write(b, last)
                    b_counter.inc()

            if crc:
                with counter.sub('crc') as c_counter:
                    for b in crc_computed.to_bytes(2, 'little', signed=False):
                        self.duplex_write(b, 0)
                        c_counter.inc()
            else:
                self.duplex_write(0x00, last)
                counter.inc()

            self.epilogue(crc_computed)
            return crc_computed

    def write_segmented(self, data: bytes, crc: bool = False, segment_size: int = 16, block_size: int = 256,
                        delay_block: float = 0.05, delay_epilogue: float = 0.5) -> int:
        last = 0
        crc_computed = 0xFFFF

        with ByteCounter('data') as counter:
            with counter.sub('bytes') as b_counter:
                for bb in [data[i:i + segment_size] for i in range(0, len(data), segment_size)]:
                    for b in bb:
                        crc_computed = crc16_step(b, crc_computed)
                        last = self.duplex_write(b, last)
                        b_counter.inc()

                    if len(bb) == segment_size:
                        last = self.duplex_write(0, last)
                        counter.inc()
                        last = self.duplex_write(0, last)  # TODO repeat when duplex error CC
                        counter.inc()

                    pos = b_counter.value()
                    if pos > 0 and pos % block_size == 0:
                        sleep(delay_block)

            if crc:
                sleep(delay_block)
                with counter.sub('crc') as c_counter:
                    for b in crc_computed.to_bytes(2, 'little', signed=False):
                        self.duplex_write(b, last)
                        last = 0
                        c_counter.inc()
            else:
                self.duplex_write(0x00, last)
                counter.inc()

            sleep(delay_epilogue)
            self.epilogue(crc_computed)

        return crc_computed

    def write_data(self, data: bytes, crc: bool = False):
        self.write(len(data).to_bytes(4, 'little', signed=False), False, False)
        sleep(0.5)
        return self.write_segmented(data, crc, delay_epilogue=1.)

    def read(self, length: Optional[int] = None, crc: bool = False) -> bytes:
        buf = bytearray()
        crc_buf = bytearray()

        if length is None:
            length = self.read_word()
            crc_buf.extend(length.to_bytes(2, 'little'))

        for i in range(length):
            buf.append(self.transfer(0x00))

        # CRC
        if crc:
            crc_received = self.read_word()
            crc_buf.extend(buf)
            crc_computed = crc16_quick(crc_buf)
            if crc_received != crc_computed:
                raise CRCError(crc_computed, crc_received)

        return bytes(buf)

    def read_command(self, cmd: int, length: Optional[int] = None, crc: bool = False) -> bytes:
        self.transfer(cmd)
        return self.read(length, crc)

    def greedy_read_command(self, cmd: int) -> bytes:
        buf = bytearray()
        try:
            self.transfer(cmd)
            sleep(0.05)
            initial_response = self.transfer(0x00)
            if initial_response == 0x03:
                raise StopReading()
            if initial_response != 0x00:
                raise CommunicationError('Failed opening directory')

            while True:
                buf.append(self.transfer(0x00))
                buf.append(self.transfer(0x00))

                while True:
                    if len(buf) > 5 and sum(buf[-5:]) == 0:
                        raise StopReading()
                    if len(buf) > 5 and buf[-1] == 0xFF and buf[-2] == 0xFF and buf[-3] == 0xFF:
                        raise StopReading()

                    b = self.transfer(0x00)
                    buf.append(b)
                    if b == 0x00:
                        sleep(0.05)
                        self.transfer(0x00)
                        break
        except StopReading:
            pass

        return bytes(buf.rstrip(b'\xff'))

    def exec_command(self, cmd: int) -> None:
        self.transfer(cmd)
        self.duplex_write(0, cmd)

    def write_command(self, cmd: int, data: bytes, var_len: bool = False, crc: bool = False) -> None:
        self.transfer(cmd)
        self.write(data, var_len, crc)

    def upload_command(self, cmd: int, data: bytes, crc: bool = False):
        self.transfer(cmd)
        return self.write_data(data, crc)

    def upload_file_command(self, cmd: int, name: str, data: bytes, crc: bool = False):
        self.transfer(cmd)
        self.write(name.encode('ascii'), True, False)
        return self.write_data(data, crc)
