from __future__ import annotations
from time import sleep
from typing import TYPE_CHECKING
from extras import bus
from .transaction import SPITransaction

if TYPE_CHECKING:
    from configfile import ConfigWrapper
    from klippy import Printer
    from pins import PrinterPins
    from mcu import MCU_digital_out


class SPI:
    def __init__(self, config: ConfigWrapper):
        self.printer: Printer = config.get_printer()
        ppins: PrinterPins = self.printer.lookup_object('pins')

        cs_pin = config.get('cs_pin')
        spi_bus = config.get('spi_bus')
        speed = config.getint('spi_speed', 300000, minval=100000)

        self._cs: MCU_digital_out = ppins.setup_pin('digital_out', cs_pin)
        self._cs.setup_max_duration(0)
        self._cs.setup_start_value(1., 1.)
        self._spi = bus.MCU_SPI(self._cs.get_mcu(), spi_bus, None, 3, speed)

    def _switch_cs(self, active: bool):
        print_time = self._cs.get_mcu().estimated_print_time(self.printer.reactor.monotonic())
        self._cs.set_digital(print_time + 0.001, 0 if active else 1)
        sleep(0.005)  # TODO

    def __enter__(self):
        self._switch_cs(True)
        return SPITransaction(self._spi)

    def __exit__(self, exc_type, exc_val, exc_tb):
        self._switch_cs(False)
