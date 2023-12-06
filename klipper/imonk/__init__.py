from __future__ import annotations
from typing import TYPE_CHECKING
from .spi import SPI
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

        self.gcode.register_command('IMONK_VERSION', self.cmd_IMONK_VERSION, desc="Print current IMONK firmware version")
        self.gcode.register_command('IMONK_TEST', self.cmd_IMONK_TEST, desc="Test")
        self.gcode.register_command('IMONK_TEST2', self.cmd_IMONK_TEST2, desc="Test2")

    def cmd_IMONK_VERSION(self, gcmd: GCodeCommand):
        with self.spi as spi:
            try:
                version = spi.read_command(0x01, 3)
                gcmd.respond_info(f'IMONK Firmware Version {version[0]}.{version[1]}.{version[2]}')
            except CommunicationError as e:
                raise gcmd.error(str(e))

    def cmd_IMONK_TEST(self, gcmd: GCodeCommand):
        with self.spi as spi:
            try:
                response = spi.read_command(0xA1).decode('ascii')
                gcmd.respond_info(f'Answer: {response}')
            except CommunicationError as e:
                raise gcmd.error(str(e))

    def cmd_IMONK_TEST2(self, gcmd: GCodeCommand):
        with self.spi as spi:
            try:
                spi.write_command(0xA0, gcmd.get('TEXT', 'TEST123').encode('ascii'))
            except CommunicationError as e:
                raise gcmd.error(str(e))


def load_config(config: ConfigWrapper):
    return IMONK(config)
