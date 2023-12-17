from __future__ import annotations

import struct
from time import sleep
from typing import TYPE_CHECKING
from os.path import expanduser
from base64 import b64decode
import traceback
from .spi import SPI
from .spi.error import CommunicationError
from .state import State

if TYPE_CHECKING:
    from gcode import GCodeCommand
    from configfile import ConfigWrapper
    from klippy import Printer
    from gcode import GCodeDispatch
    from webhooks import WebHooks, WebRequest, ClientConnection
    from .resource import IMONKResourceImage, IMONKResourceScene


class IMONKManager:
    def __init__(self, config: ConfigWrapper):
        self.state = State()
        self.printer: Printer = config.get_printer()
        self.gcode: GCodeDispatch = self.printer.lookup_object('gcode')
        self.spi = SPI(config)

        self.printer.register_event_handler("klippy:connect", self.cmd_IMONK_SYNCHRONIZE)  # TODO config

        webhooks: WebHooks = self.printer.lookup_object('webhooks')
        webhooks.register_endpoint('imonk/version', self.handle_webhook_version)
        webhooks.register_endpoint('imonk/update', self.handle_webhook_update)

        self.gcode.register_command('IMONK_FIRMWARE_VERSION', self.cmd_IMONK_FIRMWARE_VERSION, desc='Print current IMONK firmware version')
        self.gcode.register_command('IMONK_FIRMWARE_UPDATE', self.cmd_IMONK_FIRMWARE_UPDATE, desc='Update IMONK firmware')
        self.gcode.register_command('IMONK_SYNCHRONIZE', self.cmd_IMONK_SYNCHRONIZE, desc='Synchronize IMONK scenes and images')
        self.gcode.register_command('IMONK_REBOOT', self.cmd_IMONK_REBOOT, desc='Reboot IMONK')

        self.gcode.register_command('IMONK_TEST', self.cmd_IMONK_TEST, desc="Test")
        self.gcode.register_command('IMONK_TEST2', self.cmd_IMONK_TEST2, desc="Test2")
        self.gcode.register_command('IMONK_TEST3', self.cmd_IMONK_TEST3, desc="Test3")
        self.gcode.register_command('IMONK_TEST4', self.cmd_IMONK_TEST4, desc="Upload image")
        self.gcode.register_command('IMONK_TEST5', self.cmd_IMONK_TEST5, desc="Remove image")

# API
    def get_version(self) -> tuple[int, int, int]:
        with self.spi as spi:
            try:
                version = spi.read_command(0x01, 3)
                return version[0], version[1], version[2]
            except CommunicationError as e:
                raise self.gcode.error(str(e))

    def update_firmware(self, data: bytes) -> None:
        with self.spi as spi:
            try:
                spi.upload_command(0x0A, data, True)
            except (CommunicationError, FileNotFoundError) as e:
                raise self.gcode.error(str(e))

    def remove_image(self, name: str):
        with self.spi as spi:
            try:
                spi.write_command(0x10, name.encode('ascii'), True, True)
                del self.state.device.images[name]
            except (CommunicationError, FileNotFoundError) as e:
                raise self.gcode.error(str(e))

    def images_manifest(self) -> dict:
        result = dict()
        with self.spi as spi:
            try:
                buf = spi.greedy_read_command(0x12)
                while len(buf):
                    checksum, = struct.unpack("<H", buf[:2])
                    name, buf = buf[2:].split(b'\x00', maxsplit=1)
                    result[name.decode('ascii')] = checksum
            except Exception as e:
                raise self.gcode.error(str(e) + '\n' + traceback.format_exc())

        self.state.device.images = result
        return result

    def upload_image(self, name: str, data: bytes) -> int:
        with self.spi as spi:
            try:
                crc = spi.upload_file_command(0x11, name, data, True)
                self.state.device.images[name] = crc
                return crc
            except (CommunicationError, FileNotFoundError) as e:
                raise self.gcode.error(str(e))

    def remove_scene(self, name: str):
        with self.spi as spi:
            try:
                spi.write_command(0x20, name.encode('ascii'), True, True)
                del self.state.device.scenes[name]
            except (CommunicationError, FileNotFoundError) as e:
                raise self.gcode.error(str(e) + '\n' + traceback.format_exc())

    def scenes_manifest(self) -> dict:
        result = dict()
        with self.spi as spi:
            try:
                buf = spi.greedy_read_command(0x22)
                while len(buf):
                    checksum, = struct.unpack("<H", buf[:2])
                    name, buf = buf[2:].split(b'\x00', maxsplit=1)
                    result[name.decode('ascii')] = checksum
            except Exception as e:
                raise self.gcode.error(str(e) + '\n' + traceback.format_exc())

        self.state.device.scenes = result
        return result

    def upload_scene(self, name: str, data: str) -> int:
        with self.spi as spi:
            try:
                crc = spi.upload_file_command(0x21, name, data.encode('ascii'), True)
                self.state.device.scenes[name] = crc
                return crc
            except (CommunicationError, FileNotFoundError) as e:
                raise self.gcode.error(str(e) + '\n' + traceback.format_exc())

    def reboot(self) -> None:
        with self.spi as spi:
            try:
                spi.exec_command(0x0F)
            except (CommunicationError, FileNotFoundError) as e:
                raise self.gcode.error(str(e))

# Webhooks
    def handle_webhook_version(self, web_request: WebRequest):
        major, minor, micro = self.get_version()
        web_request.send({'major': major, 'minor': minor, 'micro': micro})

    def handle_webhook_update(self, web_request: WebRequest):
        conn: ClientConnection = web_request.get_client_connection()
        data = b64decode(web_request.get_str('firmware'))
        conn.send({'params': {'progress': {'info': 'Updating Firmware'}}})
        self.update_firmware(data)
        web_request.send({'info': 'Firmware update continues on the device'})

# Commands
    def cmd_IMONK_FIRMWARE_VERSION(self, gcmd: GCodeCommand):
        major, minor, micro = self.get_version()
        gcmd.respond_info(f'IMONK Firmware Version {major}.{minor}.{micro}')

    def cmd_IMONK_FIRMWARE_UPDATE(self, gcmd: GCodeCommand):
        # TODO bail in print
        path = gcmd.get('PATH')
        with open(expanduser(path), 'rb') as f:
            gcmd.respond_info(f'Updating Firmware from: {path}')
            self.update_firmware(f.read())
            gcmd.respond_info('Firmware update continues on the device')

    def cmd_IMONK_REBOOT(self, gcmd: GCodeCommand):
        self.reboot()
        gcmd.respond_info('Rebooting IMONK')

    # TODO SET_SCENE
    # TODO SET_VARIABLE
    def cmd_IMONK_TEST(self, gcmd: GCodeCommand):
        gcmd.respond_info(str(self.images_manifest()))

    def cmd_IMONK_TEST2(self, gcmd: GCodeCommand):
        gcmd.respond_info(str(self.scenes_manifest()))

    def cmd_IMONK_TEST3(self, gcmd: GCodeCommand):
        with self.spi as spi:
            try:
                spi.write_command(0xA2, b'\x00\x00\x01', False, False)
            except CommunicationError as e:
                raise gcmd.error(str(e))

    def cmd_IMONK_TEST4(self, gcmd: GCodeCommand):
        path = gcmd.get('PATH')
        name = gcmd.get('NAME')
        with open(expanduser(path), 'rb') as f:
            gcmd.respond_info(f'Uploading image {name} from {path}')
            crc = self.upload_image(name, f.read())
            gcmd.respond_info(f'Image uploaded successfully CRC {crc}')
            sleep(1)
            self.reboot()

    def cmd_IMONK_TEST5(self, gcmd: GCodeCommand):
        name = gcmd.get('NAME')
        gcmd.respond_info(f'Removing image {name}')
        self.remove_image(name)
        gcmd.respond_info('Image removed successfully')
        sleep(1)
        self.reboot()

# Sync
    def cmd_IMONK_SYNCHRONIZE(self, *_):
        self.images_manifest()
        sleep(0.1)
        self.scenes_manifest()
        sleep(0.1)

        # TODO Fix logs at startup
        for name in self.state.device.images.keys():
            if name not in self.state.host.images:
                # self.remove_image(name) TODO
                sleep(0.1)
                self.gcode.respond_info(f'IMONK Removed image {name} from device')

        for name in self.state.device.scenes.keys():
            if name not in self.state.host.scenes:
                # self.remove_scene(name) TODO
                sleep(0.1)
                self.gcode.respond_info(f'IMONK Removed scene {name} from device')

        for name, resource in self.state.host.scenes.items():
            dev_crc = self.state.device.scenes.get(name, -1)
            if dev_crc != resource.crc:
                self.gcode.respond_info(f'IMONK Scene CRC mismatch Host {resource.crc}, Device {dev_crc}')
                self.upload_scene(name, resource.data)
                sleep(0.1)
                if dev_crc == -1:
                    self.gcode.respond_info(f'IMONK Uploaded scene {name}')
                else:
                    self.gcode.respond_info(f'IMONK Updated scene {name}')

        for name, resource in self.state.host.images.items():
            dev_crc = self.state.device.images.get(name, -1)
            if dev_crc != resource.crc:
                self.gcode.respond_info(f'IMONK Image CRC mismatch Host {resource.crc}, Device {dev_crc}')
                self.upload_image(name, resource.data)
                sleep(0.1)
                if dev_crc == -1:
                    self.gcode.respond_info(f'IMONK Uploaded image {name}')
                else:
                    self.gcode.respond_info(f'IMONK Updated image {name}')

        self.gcode.respond_info('IMONK Synchronized!')
