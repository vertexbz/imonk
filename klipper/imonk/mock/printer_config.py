from __future__ import annotations
from .printer import Printer
from ..klipper import PrinterConfig as PrinterConfigBase


class PrinterConfig(PrinterConfigBase):
    def __init__(self, printer):
        super().__init__(Printer())
        self.printer = printer
