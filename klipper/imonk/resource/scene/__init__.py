from __future__ import annotations
from typing import TYPE_CHECKING, Union
from ast import literal_eval
from json import dumps as to_json
from configfile import error as ConfigError
from ..spi.crc import crc16_quick
from .config import Scene as SceneConfig
from .config.widget.image import ImageWidget

if TYPE_CHECKING:
    from configfile import ConfigWrapper


class Slot:
    def get_type(self) -> type:
        return int

    def get_id(self) -> int:
        return 1


class IMONKResourceScene:
    def __init__(self, config: ConfigWrapper, name: str):
        background = config.get('background')
        widgets = literal_eval(config.get('widgets'))
        if not isinstance(widgets, list):
            raise ConfigError(f'Invalid IMONK scene {name} configuration')

        self._config = SceneConfig(background=background, widgets=widgets)
        self._data = to_json(self._config.dump()).encode('ascii')
        self._crc = crc16_quick(self._data)
        self.name = name

    @property
    def crc(self) -> int:
        return self._crc

    @property
    def data(self) -> bytes:
        return self._data

    @property
    def used_images(self) -> set[str]:
        return {w.name for w in self._config.widgets if isinstance(w, ImageWidget)}

    def get_slot(self, slot_name: str) -> Slot:
        raise ValueError(f'Unknown variable {slot_name}')

    def __eq__(self, other):
        if other == self.crc:
            return True
        return super().__eq__(other)
