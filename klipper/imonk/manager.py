from __future__ import annotations
from ast import literal_eval
from typing import TYPE_CHECKING
from os.path import expanduser
from base64 import b64decode
from .api import API
from .spi import SPI
from .state import State
from .utils import with_error_handler
from .resource import build_resource, IMONKResourceImage, IMONKResourceScene
from .mock.printer_config import PrinterConfig

if TYPE_CHECKING:
    from configfile import ConfigWrapper
    from klippy import Printer
    from gcode import GCodeDispatch, GCodeCommand
    from extras.idle_timeout import IdleTimeout
    from webhooks import WebHooks, WebRequest, ClientConnection


class IMONKManager:
    def __init__(self, config: ConfigWrapper):
        self.state = State()
        self.api = API(SPI(config), self.state)

        self.printer: Printer = config.get_printer()
        self.idle_timeout: IdleTimeout = self.printer.load_object(config, 'idle_timeout')

        if config.getboolean('synchronize', True):
            self.printer.register_event_handler("klippy:ready", self.on_klippy_ready)

        webhooks: WebHooks = self.printer.lookup_object('webhooks')
        webhooks.register_endpoint('imonk/version', self.handle_webhook_version)
        webhooks.register_endpoint('imonk/update', self.handle_webhook_update)

        self.gcode: GCodeDispatch = self.printer.lookup_object('gcode')
        self.gcode.register_command(
            'IMONK_FIRMWARE_VERSION',
            with_error_handler(self.cmd_IMONK_FIRMWARE_VERSION),
            desc='Print current IMONK firmware version'
        )
        self.gcode.register_command(
            'IMONK_FIRMWARE_UPDATE',
            with_error_handler(self.cmd_IMONK_FIRMWARE_UPDATE),
            desc='Update IMONK firmware'
        )
        self.gcode.register_command(
            'IMONK_STAGE_SCENE',
            with_error_handler(self.cmd_IMONK_STAGE_SCENE),
            desc="Stages IMONK Scene for variables input and display"
        )
        self.gcode.register_command(
            'IMONK_SET_VALUE',
            with_error_handler(self.cmd_IMONK_SET_VALUE),
            desc="Set IMONK Scene variable value"
        )
        self.gcode.register_command(
            'IMONK_COMMIT_SCENE',
            with_error_handler(self.cmd_IMONK_COMMIT_SCENE),
            desc="Commits (displays) prepared IMONK Scene"
        )
        self.gcode.register_command(
            'IMONK_ABORT_SCENE',
            with_error_handler(self.cmd_IMONK_ABORT_SCENE),
            desc="Aborts IMONK Scene preparation"
        )
        self.gcode.register_command(
            'IMONK_REBOOT',
            with_error_handler(self.cmd_IMONK_REBOOT),
            desc='Reboot IMONK'
        )
        self.gcode.register_command(
            'IMONK_RELOAD_CONFIG',
            self.cmd_IMONK_RELOAD_CONFIG,
            desc='(Re)Load IMONK configuration'
        )
        self.gcode.register_command(
            'IMONK_LOAD_DEVICE_STATE',
            with_error_handler(self.cmd_IMONK_LOAD_DEVICE_STATE),
            desc='Load IMONK scenes and images from device'
        )
        self.gcode.register_command(
            'IMONK_SYNCHRONIZE',
            with_error_handler(self.cmd_IMONK_SYNCHRONIZE),
            desc='Synchronize IMONK scenes and images'
        )

        self._load_resources(config)

# Hooks
    def on_klippy_ready(self):
        command = self.gcode.create_gcode_command('', '', {'QUIET': 1})

        self.cmd_IMONK_LOAD_DEVICE_STATE(command)
        self.cmd_IMONK_SYNCHRONIZE(command)
        command.respond_info('IMONK Active')

    def get_status(self, *_) -> dict:
        return {
            'scene': {
                'current': self.state.scene.current,
                'staged': self.state.scene.staged
            }
        }

# Webhooks
    def handle_webhook_version(self, web_request: WebRequest):
        major, minor, micro = self.api.get_version()
        web_request.send({'major': major, 'minor': minor, 'micro': micro})

    def handle_webhook_update(self, web_request: WebRequest):
        conn: ClientConnection = web_request.get_client_connection()
        data = b64decode(web_request.get_str('firmware'))
        conn.send({'params': {'progress': {'info': 'Updating Firmware'}}})
        self.api.update_firmware(data)
        web_request.send({'info': 'Firmware update continues on the device'})

# Commands
    def cmd_IMONK_FIRMWARE_VERSION(self, gcmd: GCodeCommand):
        major, minor, micro = self.api.get_version()
        gcmd.respond_info(f'IMONK Firmware Version {major}.{minor}.{micro}')

    def cmd_IMONK_FIRMWARE_UPDATE(self, gcmd: GCodeCommand):
        if self._is_printing():
            raise gcmd.error('Cannot update IMONK while printing')

        path = gcmd.get('PATH')
        with open(expanduser(path), 'rb') as f:
            gcmd.respond_info(f'Updating Firmware from: {path}')
            self.api.update_firmware(f.read())
            gcmd.respond_info('Firmware update continues on the device')

    def cmd_IMONK_STAGE_SCENE(self, gcmd: GCodeCommand):
        name = gcmd.get('NAME')
        scene_id = self.api.scene_stage(name)
        gcmd.respond_info(f'IMONK staged scene {name} with SID={scene_id}')

    def cmd_IMONK_SET_VALUE(self, gcmd: GCodeCommand):
        sid = gcmd.get_int('SID')
        slot = gcmd.get('SLOT')
        value = gcmd.get('VALUE')
        try:
            value = literal_eval(value)
        except ValueError:
            pass

        if not isinstance(value, (str, int, float, bool)):
            raise gcmd.error('Only string, integer, float and boolean values allowed')

        self.api.scene_set_value(sid, slot, value)
        gcmd.respond_info('IMONK scene value updated')

    def cmd_IMONK_COMMIT_SCENE(self, gcmd: GCodeCommand):
        sid = gcmd.get_int('SID')
        self.api.scene_commit(sid)
        gcmd.respond_info('IMONK committed scene')

    def cmd_IMONK_ABORT_SCENE(self, gcmd: GCodeCommand):
        sid = gcmd.get_int('SID')
        self.api.scene_commit(sid)
        gcmd.respond_info('IMONK aborted scene setup')

    def cmd_IMONK_REBOOT(self, gcmd: GCodeCommand):
        self.api.reboot()
        gcmd.respond_info('Rebooting IMONK')

    def cmd_IMONK_RELOAD_CONFIG(self, gcmd: GCodeCommand):
        try:
            config = PrinterConfig(self.printer).read_main_config()
            self._load_resources(config)
            gcmd.respond_info('IMONK config loaded!' + self._diff_message())
        except Exception as e:
            raise gcmd.error(f'Failed reloading IMONK configuration: {e}')

    def cmd_IMONK_LOAD_DEVICE_STATE(self, gcmd: GCodeCommand):
        quiet = gcmd.get_int('QUIET', 0) == 1

        self.api.images_manifest()
        self.api.scenes_manifest()
        if not quiet:
            gcmd.respond_info('IMONK State loaded!' + self._diff_message())

    def cmd_IMONK_SYNCHRONIZE(self, gcmd: GCodeCommand):
        quiet = gcmd.get_int('QUIET', 0) == 1

        if self._is_printing():
            raise gcmd.error('Cannot synchronize IMONK while printing')

        known_images = set(self.state.host.images.keys())

        for scene in self.state.host.scenes.values():
            diff = scene.used_images.difference(known_images)
            if diff:
                raise gcmd.error(f'IMONK Scene {scene.name} references not existing images {diff}')

        for name in self.state.device.images.keys():
            if name not in self.state.host.images:
                # self.api.remove_image(name) TODO
                if not quiet:
                    gcmd.respond_info(f'IMONK Removed unused image {name} from device')

        for name in self.state.device.scenes.keys():
            if name not in self.state.host.scenes:
                # self.api.remove_scene(name) TODO
                if not quiet:
                    gcmd.respond_info(f'IMONK Removed unused scene {name} from device')

        for name, resource in self.state.host.scenes.items():
            dev_crc = self.state.device.scenes.get(name, -1)
            if dev_crc != resource.crc:
                if not quiet:
                    gcmd.respond_info(f'IMONK Scene CRC mismatch Host {resource.crc}, Device {dev_crc}')
                self.api.upload_scene(name, resource.data)
                if not quiet:
                    if dev_crc == -1:
                        gcmd.respond_info(f'IMONK Uploaded scene {name}')
                    else:
                        gcmd.respond_info(f'IMONK Updated scene {name}')

        for name, resource in self.state.host.images.items():
            dev_crc = self.state.device.images.get(name, -1)
            if dev_crc != resource.crc:
                if not quiet:
                    gcmd.respond_info(f'IMONK Image CRC mismatch Host {resource.crc}, Device {dev_crc}')
                self.api.upload_image(name, resource.data)
                if not quiet:
                    if dev_crc == -1:
                        gcmd.respond_info(f'IMONK Uploaded image {name}')
                    else:
                        gcmd.respond_info(f'IMONK Updated image {name}')
        if not quiet:
            gcmd.respond_info('IMONK Synchronized!')

# Helpers
    def _load_resources(self, config: ConfigWrapper):
        images = {}
        scenes = {}
        for section in config.get_prefix_sections('imonk '):
            resource = build_resource(section)
            if isinstance(resource, IMONKResourceImage):
                images[resource.name] = resource
            elif isinstance(resource, IMONKResourceScene):
                scenes[resource.name] = resource
            else:
                raise ValueError(f'Unknown resource type {type(resource)}')

        self.state.host.images = images
        self.state.host.scenes = scenes

    def _is_printing(self) -> bool:
        return self.idle_timeout.state == "Printing"

    def _diff_message(self) -> str:
        diff_images = self.state.host.images != self.state.device.images
        diff_scenes = self.state.host.scenes != self.state.device.scenes

        rest = "seem to differ between Klipper and the device, use IMONK_SYNCHRONIZE to get them back in sync"
        if diff_images and diff_scenes:
            return "\nImages and Scenes " + rest
        if diff_images:
            return "\nImages " + rest
        if diff_scenes:
            return "\nScenes " + rest

        return ""
