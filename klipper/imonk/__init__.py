from __future__ import annotations
from typing import TYPE_CHECKING
from .imonk import IMONK

if TYPE_CHECKING:
    from configfile import ConfigWrapper


def load_config(config: ConfigWrapper):
    return IMONK(config)
