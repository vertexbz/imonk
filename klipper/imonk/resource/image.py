from __future__ import annotations
from typing import TYPE_CHECKING
from os.path import expanduser, exists, getmtime
from ..klipper import ConfigError
from ..spi.crc import crc16_step

if TYPE_CHECKING:
    from ..klipper import ConfigWrapper


class IMONKResourceImage:
    def __init__(self, config: ConfigWrapper, name: str):
        image_path = config.get('path')
        abs_path = expanduser(image_path)
        if not exists(abs_path):
            raise ConfigError(f'File {image_path} cannot be found')

        self._crc = (-1.0, 0)
        self._path = abs_path
        self.path = image_path
        self.name = name

    @property
    def crc(self) -> int:
        mtime = getmtime(self._path)
        if mtime != self._crc[0]:
            with open(self._path, 'rb') as f:
                crc = 0xFFFF
                while True:
                    bytes_in = f.read(1)
                    if not bytes_in:
                        break
                    byte = bytes_in[0]
                    crc = crc16_step(byte, crc)
                self._crc = (mtime, crc & 0xFFFF)

        return self._crc[1]

    @property
    def data(self) -> bytes:
        with open(self._path, 'rb') as f:
            return f.read()

    def __eq__(self, other):
        if other == self.crc:
            return True
        return super().__eq__(other)
