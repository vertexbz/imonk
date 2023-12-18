from __future__ import annotations
from typing import TYPE_CHECKING, Union
from ast import literal_eval
from json import dumps as to_json
from configfile import error as ConfigError
from ..spi.crc import crc16_quick

if TYPE_CHECKING:
    from configfile import ConfigWrapper
    from ..manager import IMONKManager


def validate_scene(scene: Union[dict, list]) -> None:
    pass


def normalize_scene(scene: Union[dict, list]) -> bytes:
    return to_json(scene).encode('ascii')


class Slot:
    def get_type(self) -> type:
        return int

    def get_id(self) -> int:
        return 1


class IMONKResourceScene:
    def __init__(self, config: ConfigWrapper, name: str, manager: IMONKManager):
        scene = literal_eval(config.get('scene'))
        if not isinstance(scene, (dict, list)):
            raise ConfigError(f'Invalid INMONK scene {name} configuration')

        validate_scene(scene)
        self._data = normalize_scene(scene)
        self._crc = crc16_quick(self._data)
        self.name = name

        manager.state.host.scenes[self.name] = self

    @property
    def crc(self) -> int:
        return self._crc

    @property
    def data(self) -> bytes:
        return self._data

    @property
    def used_images(self) -> set[str]:
        return set()

    def get_slot(self, slot_name: str) -> Slot:
        raise ValueError(f'Unknown variable {slot_name}')
