from __future__ import annotations
from typing import TYPE_CHECKING
from .resource import build_resource
from .manager import IMONKManager

if TYPE_CHECKING:
    from configfile import ConfigWrapper
    from klippy import Printer


def load_config(config: ConfigWrapper):
    return IMONKManager(config)


def load_config_prefix(config: ConfigWrapper):
    printer: Printer = config.get_printer()
    manager = printer.load_object(config, 'imonk')
    return build_resource(config, manager)
