from __future__ import annotations
from typing import TYPE_CHECKING, Union
import struct

if TYPE_CHECKING:
    from .spi import SPI
    from .state import State
    from .resource import IMONKResourceScene


class API:
    def __init__(self, spi: SPI, state: State):
        self.spi = spi
        self.state = state

    def get_version(self) -> tuple[int, int, int]:
        with self.spi as spi:
            version = spi.read_command(0x01, 3, True)
            return version[0], version[1], version[2]

    def update_firmware(self, data: bytes) -> None:
        with self.spi as spi:
            spi.upload_command(0x0A, data, True)

    def remove_image(self, name: str) -> None:
        with self.spi as spi:
            spi.write_command(0x10, name.encode('ascii'), True, True)
            del self.state.device.images[name]

    def images_manifest(self) -> dict:
        result = dict()
        with self.spi as spi:
            buf = spi.greedy_read_command(0x12)
            while len(buf):
                checksum, = struct.unpack("<H", buf[:2])
                name, buf = buf[2:].split(b'\x00', maxsplit=1)
                result[name.decode('ascii')] = checksum

            self.state.device.images = result

        return result

    def upload_image(self, name: str, data: bytes) -> int:
        with self.spi as spi:
            crc = spi.upload_file_command(0x11, name, data, True)
            self.state.device.images[name] = crc
            return crc

    def remove_scene(self, name: str) -> None:
        with self.spi as spi:
            spi.write_command(0x20, name.encode('ascii'), True, True)
            del self.state.device.scenes[name]

    def scenes_manifest(self) -> dict:
        result = dict()
        with self.spi as spi:
            buf = spi.greedy_read_command(0x22)
            while len(buf):
                checksum, = struct.unpack("<H", buf[:2])
                name, buf = buf[2:].split(b'\x00', maxsplit=1)
                result[name.decode('ascii')] = checksum

            self.state.device.scenes = result

        return result

    def upload_scene(self, name: str, data: str) -> int:
        with self.spi as spi:
            crc = spi.upload_file_command(0x21, name, data.encode('ascii'), True)
            self.state.device.scenes[name] = crc
            return crc

    def reboot(self) -> None:
        with self.spi as spi:
            spi.exec_command(0x0F)

    def scene_stage(self, name: str) -> int:
        with self.spi as spi:
            spi.write_command(0x50, name.encode('ascii'), True, False)
            scene_id = spi.read(1, False)
            spi.epilogue(0)
            self.state.scene.staged = (scene_id, self.state.host.scenes[name])
            return scene_id

    def scene_set_value(self, scene_id: int, var_name: str, value: Union[str, float, int, bool]) -> None:
        scene = self._scene_by_id(scene_id)
        slot = scene.get_slot(var_name)
        if not isinstance(value, slot.get_type()):
            raise ValueError(f'Scene {scene.name} variable {var_name} should be of type {slot.get_type()}')

        with self.spi as spi:
            spi.write_command(0x51, scene_id.to_bytes(1, 'little'), False, False)

    def scene_commit(self, scene_id: int) -> None:
        with self.spi as spi:
            spi.write_command(0x5E, scene_id.to_bytes(1, 'little'), False, False)
            self.state.scene.current = self.state.scene.staged
            self.state.scene.staged = None

    def scene_abort(self, scene_id: int) -> None:
        with self.spi as spi:
            spi.write_command(0x5F, scene_id.to_bytes(1, 'little'), False, False)
            self.state.scene.staged = None

    def _scene_by_id(self, scene_id: int) -> IMONKResourceScene:
        if self.state.scene.current is not None and scene_id == self.state.scene.current[0]:
            return self.state.scene.current[1]
        if self.state.scene.staged is not None and scene_id == self.state.scene.staged[0]:
            return self.state.scene.staged[1]
        raise ValueError(f'Invalid SID {scene_id}')
