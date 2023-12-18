from __future__ import annotations
from typing import TYPE_CHECKING
from os.path import expanduser, exists
from configfile import error as ConfigError
from ..spi.crc import crc16_step

if TYPE_CHECKING:
    from configfile import ConfigWrapper
    from ..manager import IMONKManager


class IMONKResourceImage:
    def __init__(self, config: ConfigWrapper, name: str, manager: IMONKManager):
        image_path = config.get('path')
        if not exists(expanduser(image_path)):
            raise ConfigError(f'File {image_path} cannot be found')

        self.path = image_path
        self.name = name

        manager.state.host.images[self.name] = self

    @property
    def crc(self) -> int:
        with open(expanduser(self.path), 'rb') as f:
            crc = 0xFFFF
            while True:
                bytes_in = f.read(1)
                if not bytes_in:
                    break
                byte = bytes_in[0]
                crc = crc16_step(byte, crc)

            return crc & 0xFFFF

    @property
    def data(self) -> bytes:
        with open(expanduser(self.path), 'rb') as f:
            return f.read()
