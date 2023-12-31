from __future__ import annotations
from time import sleep
from typing import TYPE_CHECKING
from typing import Optional
from ..klipper import MCU_SPI
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
        self._cs: MCU_digital_out = ppins.setup_pin('digital_out', cs_pin)
        self._cs.setup_max_duration(0)
        self._cs.setup_start_value(1., 1.)

        sw_pins = None
        spi_bus = config.get('spi_bus', None)
        if spi_bus is None:
            sw_pin_names = [f'spi_software_{name}_pin' for name in ['miso', 'mosi', 'sclk']]
            sw_pin_params = [ppins.lookup_pin(config.get(name), share_type=name) for name in sw_pin_names]
            for pin_params in sw_pin_params:
                if pin_params['chip'] != self._cs.get_mcu():
                    raise ppins.error(f"{config.get_name()}: spi pins must be on same mcu" )
            sw_pins = tuple([pin_params['pin'] for pin_params in sw_pin_params])

        speed = config.getint('spi_speed', 300000, minval=100000)
        self._spi = MCU_SPI(self._cs.get_mcu(), spi_bus, None, 3, speed, sw_pins)

    def _switch_cs(self, active: bool):
        print_time = self._cs.get_mcu().estimated_print_time(self.printer.reactor.monotonic())
        self._cs.set_digital(print_time + 0.001, 0 if active else 1)
        sleep(0.005)  # TODO - should not this

    def __enter__(self):
        self._switch_cs(True)
        return SPITransaction(self._spi)

    def __exit__(self, exc_type: Optional[type], exc_val: Optional[Exception], exc_tb):
        self._switch_cs(False)
