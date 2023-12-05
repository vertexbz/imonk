from __future__ import annotations
from typing import TYPE_CHECKING
from .spi import SPI

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

        self.gcode.register_command('IMONK_VERSION', self.cmd_IMONK_VERSION, desc=self.cmd_IMONK_VERSION_help)

    cmd_IMONK_VERSION_help = "Print current IMONK firmware version"

    def cmd_IMONK_VERSION(self, gcmd: GCodeCommand):
        with self.spi as spi:
            response = spi.read_command(0x01, 3)
            gcmd.respond_info(f'IMONK Firmware Version {response[0]}.{response[1]}.{response[2]}')


def load_config(config: ConfigWrapper):
    return IMONK(config)
