from __future__ import annotations
from typing import TYPE_CHECKING
import os
import traceback
from .spi import SPI
from .spi.transaction import SPITransaction
from .spi.error import CommunicationError

if TYPE_CHECKING:
    from gcode import GCodeCommand
    from configfile import ConfigWrapper
    from klippy import Printer
    from gcode import GCodeDispatch


class IMONK:
    def __init__(self, config: ConfigWrapper):
        self.printer: Printer = config.get_printer()
        self.gcode: GCodeDispatch = self.printer.lookup_object('gcode')
        self.spi = SPI(config)

        self.gcode.register_command('IMONK_FIRMWARE_VERSION', self.cmd_IMONK_FIRMWARE_VERSION, desc='Print current IMONK firmware version')
        self.gcode.register_command('IMONK_FIRMWARE_CHECK_UPDATE', self.cmd_IMONK_FIRMWARE_CHECK_UPDATE, desc='Check for IMONK firmware update')
        self.gcode.register_command('IMONK_FIRMWARE_UPDATE', self.cmd_IMONK_FIRMWARE_UPDATE, desc='Update IMONK firmware')

        self.gcode.register_command('IMONK_TEST', self.cmd_IMONK_TEST, desc="Test")
        self.gcode.register_command('IMONK_TEST2', self.cmd_IMONK_TEST2, desc="Test2")
        self.gcode.register_command('IMONK_TEST3', self.cmd_IMONK_TEST3, desc="Test3")
        self.gcode.register_command('IMONK_TEST4', self.cmd_IMONK_TEST4, desc="Test4")

    def _get_version(self, spi: SPITransaction) -> tuple[int, int, int]:
        version = spi.read_command(0x01, 3)
        return version[0], version[1], version[2]

    def cmd_IMONK_FIRMWARE_VERSION(self, gcmd: GCodeCommand):
        with self.spi as spi:
            try:
                major, minor, micro = self._get_version(spi)
                gcmd.respond_info(f'IMONK Firmware Version {major}.{minor}.{micro}')
            except CommunicationError as e:
                raise gcmd.error(str(e))

    def cmd_IMONK_FIRMWARE_CHECK_UPDATE(self, gcmd: GCodeCommand):
        with self.spi as spi:
            try:
                major, minor, micro = self._get_version(spi)
                gcmd.respond_info(f'IMONK Firmware Version {major}.{minor}.{micro}')
            except CommunicationError as e:
                raise gcmd.error(str(e))

    def cmd_IMONK_FIRMWARE_UPDATE(self, gcmd: GCodeCommand):
        # TODO bail in print
        with self.spi as spi:
            try:
                major, minor, micro = self._get_version(spi)
                gcmd.respond_info(f'IMONK Firmware Version {major}.{minor}.{micro}')
            except CommunicationError as e:
                raise gcmd.error(str(e))

    # TODO SET_SCENE
    # TODO SET_VARIABLE
    def cmd_IMONK_TEST(self, gcmd: GCodeCommand):
        with self.spi as spi:
            try:
                spi.write_command(0xA0, gcmd.get('TEXT', 'TEST123').encode('ascii'), True)
            except CommunicationError as e:
                raise gcmd.error(str(e))

    def cmd_IMONK_TEST2(self, gcmd: GCodeCommand):
        with self.spi as spi:
            try:
                response = spi.read_command(0xA1).decode('ascii')
                gcmd.respond_info(f'Answer: {response}')
            except CommunicationError as e:
                raise gcmd.error(str(e))

    def cmd_IMONK_TEST3(self, gcmd: GCodeCommand):
        with self.spi as spi:
            try:
                spi.write_command(0xA2, b'\x00\x00\x01', False, False)
            except CommunicationError as e:
                raise gcmd.error(str(e))

    def cmd_IMONK_TEST4(self, gcmd: GCodeCommand):
        with self.spi as spi:
            try:
                with open(os.path.expanduser(gcmd.get('PATH')), 'rb') as f:
                    data = f.read()

                spi.upload_file_command(0x11, gcmd.get('NAME'), data, True)
            except (CommunicationError, FileNotFoundError) as e:
                raise gcmd.error(str(e) + '\n' + traceback.format_exc())


def load_config(config: ConfigWrapper):
    return IMONK(config)
