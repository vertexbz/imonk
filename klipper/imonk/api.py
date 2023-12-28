from __future__ import annotations
from typing import TYPE_CHECKING, Union
import struct
from .spi.const import CRCOnlyLength

if TYPE_CHECKING:
    from .spi import SPI
    from .state import State
    from .resource import IMONKResourceView


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

    def remove_view(self, name: str) -> None:
        with self.spi as spi:
            spi.write_command(0x20, name.encode('ascii'), True, True)
            del self.state.device.views[name]

    def views_manifest(self) -> dict:
        result = dict()
        with self.spi as spi:
            buf = spi.greedy_read_command(0x22)
            while len(buf):
                checksum, = struct.unpack("<H", buf[:2])
                name, buf = buf[2:].split(b'\x00', maxsplit=1)
                result[name.decode('ascii')] = checksum

            self.state.device.views = result

        return result

    def upload_view(self, name: str, data: bytes) -> int:
        with self.spi as spi:
            crc = spi.upload_file_command(0x21, name, data, True)
            self.state.device.views[name] = crc
            return crc

    def reboot(self) -> None:
        with self.spi as spi:
            spi.exec_command(0x0F)

    def view_stage(self, name: str) -> int:
        with self.spi as spi:
            spi.write_command(0x50, name.encode('ascii'), True, False)
            view_id = spi.read(1, True)[0]
            self.state.view.staged = (view_id, self.state.host.views[name])
            return view_id

    def view_set_value(self, view_id: int, var_name: str, value: Union[str, float, int, bool], force: bool = False) -> bool:
        view = self._view_by_id(view_id)
        slot = view.get_slot(var_name)
        if not isinstance(value, slot.get_type()):
            raise ValueError(f'View {view.name} variable {var_name} should be of type {slot.get_type().__name__}')

        if not force and slot.value == value:
            return False

        slot_id = slot.get_id()
        type_byte = slot.get_type_id()

        with self.spi as spi:
            spi.write_command(0x51, bytes([view_id]), False, False)
            spi.write(bytes([slot_id]), False, False)
            spi.write(bytes([type_byte]), False, False)
            spi.write(self._encode(value), isinstance(value, str) or CRCOnlyLength, True)

        slot.value = value
        return True

    def view_commit(self, view_id: int) -> None:
        with self.spi as spi:
            spi.write_command(0x5E, view_id.to_bytes(1, 'little'), False, False)
            self.state.view.current = self.state.view.staged
            self.state.view.staged = None

    def view_abort(self, view_id: int) -> None:
        with self.spi as spi:
            spi.write_command(0x5F, view_id.to_bytes(1, 'little'), False, False)
            self.state.view.staged = None

    def _view_by_id(self, view_id: int) -> IMONKResourceView:
        if self.state.view.current is not None and view_id == self.state.view.current[0]:
            return self.state.view.current[1]
        if self.state.view.staged is not None and view_id == self.state.view.staged[0]:
            return self.state.view.staged[1]
        raise ValueError(f'Invalid SID {view_id}')

    @staticmethod
    def _encode(value) -> bytes:
        if isinstance(value, str):
            return value.encode('ascii')
        if isinstance(value, bool):
            return value.to_bytes(1, 'little')
        if isinstance(value, int):
            return value.to_bytes(4, 'little')
        if isinstance(value, float):
            return struct.pack('<f', value)

        raise ValueError(f'Unsupported value type {type(value).__name__}')
