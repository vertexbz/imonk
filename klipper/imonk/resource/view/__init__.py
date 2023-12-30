from __future__ import annotations
from typing import TYPE_CHECKING
from ast import literal_eval
from json import dumps as to_json
from ...klipper import ConfigError
from ...spi.crc import crc16_quick
from .config import View as ViewConfig
from .config.widget.image import ImageWidget
from .slot import Slot

if TYPE_CHECKING:
    from ...klipper import ConfigWrapper


class IMONKResourceView:
    def __init__(self, config: ConfigWrapper, name: str):
        background = config.get('background')
        widgets = literal_eval(config.get('widgets'))
        if not isinstance(widgets, list):
            raise ConfigError(f'Invalid IMONK view {name} configuration')

        self._config = ViewConfig(background=background, widgets=widgets)
        self._data = to_json(self._config.dump()).encode('ascii')
        self._crc = crc16_quick(self._data)
        self.name = name

        enumerated_slots = enumerate(slot for w in self._config.widgets for slot in w.get_slots())
        self._slots = {name.lower(): Slot(id, typ, def_val) for (id, (name, typ, def_val)) in enumerated_slots}

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
        return self._slots.get(slot_name.lower())

    def __eq__(self, other):
        if other == self.crc:
            return True
        return super().__eq__(other)
