from __future__ import annotations
import logging
from ast import literal_eval
from typing import TYPE_CHECKING, Optional
from os.path import expanduser
from base64 import b64decode
from .api import API
from .spi import SPI
from .state import State
from .utils import with_error_handler, FunctionSmuggler, SmugglingDict
from .resource import build_resource, IMONKResourceImage, IMONKResourceView
from .mock.printer_config import PrinterConfig

if TYPE_CHECKING:
    from configfile import ConfigWrapper
    from klippy import Printer
    from gcode import GCodeDispatch, GCodeCommand
    from extras.idle_timeout import IdleTimeout
    from webhooks import WebHooks, WebRequest, ClientConnection


class IMONK:
    def __init__(self, config: ConfigWrapper):
        self.version: Optional[str] = None
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
            'IMONK_STAGE_VIEW',
            with_error_handler(self.cmd_IMONK_STAGE_VIEW),
            desc="Stages IMONK View for variables input and display"
        )
        self.gcode.register_command(
            'IMONK_STAGE_VIEW_IF_NEEDED',
            with_error_handler(self.cmd_IMONK_STAGE_VIEW_IF_NEEDED),
            desc="Stages IMONK View for variables input and display if not currently staged nor current"
        )
        self.gcode.register_command(
            'IMONK_SET_VIEW',
            with_error_handler(self.cmd_IMONK_SET_VIEW),
            desc="Stages IMONK View for variables input and display if not currently staged nor current, "
                 "sets provided values (use current or default if omitted), and displays it"
        )
        self.gcode.register_command(
            'IMONK_SET_VALUE',
            with_error_handler(self.cmd_IMONK_SET_VALUE),
            desc="Set IMONK View variable value"
        )
        self.gcode.register_command(
            'IMONK_COMMIT_VIEW',
            with_error_handler(self.cmd_IMONK_COMMIT_VIEW),
            desc="Commits (displays) prepared IMONK View"
        )
        self.gcode.register_command(
            'IMONK_ABORT_VIEW',
            with_error_handler(self.cmd_IMONK_ABORT_VIEW),
            desc="Aborts IMONK View preparation"
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
            desc='Load IMONK views and images from device'
        )
        self.gcode.register_command(
            'IMONK_SYNCHRONIZE',
            with_error_handler(self.cmd_IMONK_SYNCHRONIZE),
            desc='Synchronize IMONK views and images'
        )

        self._load_resources(config)

# Hooks
    def on_klippy_ready(self):
        command = self.gcode.create_gcode_command('', '', {'QUIET': 1})
        try:
            major, minor, micro = self.api.get_version()
            self.cmd_IMONK_LOAD_DEVICE_STATE(command)
            self.cmd_IMONK_SYNCHRONIZE(command)

            self.version = f'{major}.{minor}.{micro}'
            command.respond_info('IMONK Active')
        except self.gcode.error as e:
            command.respond_info(str(e))
            command.respond_raw('!! IMONK Initialization failed')
        except Exception as e:
            logging.exception(str(e))
            command.respond_raw('!! IMONK Initialization failed')

    def get_status(self, *_) -> dict:
        return SmugglingDict({
            'version': self.version,
            'view': {
                'current': self._get_view_state(self.state.view.current),
                'staged': self._get_view_state(self.state.view.staged)
            },
            'stage': FunctionSmuggler(self.api.view_stage),
            'stage_if_needed': FunctionSmuggler(self._stage_if_needed),
            'set_view': FunctionSmuggler(self._set_view)
        })

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

    def cmd_IMONK_STAGE_VIEW(self, gcmd: GCodeCommand):
        quiet = gcmd.get_int('QUIET', 0) == 1

        name = gcmd.get('NAME')
        view_id = self.api.view_stage(name)
        if not quiet:
            gcmd.respond_info(f'IMONK staged view \'{name}\' with SID={view_id}')

    def cmd_IMONK_STAGE_VIEW_IF_NEEDED(self, gcmd: GCodeCommand):
        quiet = gcmd.get_int('QUIET', 0) == 1

        name = gcmd.get('NAME')
        old_staged_id = self.state.view.staged[0] if self.state.view.staged else None
        view_id, is_current = self._stage_if_needed(name)
        if not quiet:
            if is_current:
                gcmd.respond_info(f'IMONK view \'{name}\' is current with SID={view_id}')
            elif view_id == old_staged_id:
                gcmd.respond_info(f'IMONK view \'{name}\' already staged with SID={view_id}')
            else:
                gcmd.respond_info(f'IMONK staged view \'{name}\' with SID={view_id}')

    def cmd_IMONK_SET_VIEW(self, gcmd: GCodeCommand):
        parameters = gcmd.get_command_parameters().copy()
        quiet = int(parameters.pop('QUIET', 0)) == 1

        name = parameters.pop('NAME')
        data = {}

        for p, v in parameters.items():
            if not p.startswith("SLOT_"):
                continue

            data[p[5:].lower()] = self._parse_value(v)

        view_id = self._set_view(name, data)
        if not quiet:
            gcmd.respond_info(f'IMONK view \'{name}\' set with SID={view_id}')

    def cmd_IMONK_SET_VALUE(self, gcmd: GCodeCommand):
        quiet = gcmd.get_int('QUIET', 0) == 1

        sid = gcmd.get_int('SID')
        slot = gcmd.get('SLOT')
        value = self._parse_value(gcmd.get('VALUE'))

        if not isinstance(value, (str, int, float, bool)):
            raise gcmd.error('Only string, integer, float and boolean values allowed')

        result = self.api.view_set_value(sid, slot, value)
        if not quiet:
            if result:
                gcmd.respond_info('IMONK view value updated')
            else:
                gcmd.respond_info('IMONK view value unchanged')

    def cmd_IMONK_COMMIT_VIEW(self, gcmd: GCodeCommand):
        quiet = gcmd.get_int('QUIET', 0) == 1

        sid = gcmd.get_int('SID')
        self.api.view_commit(sid)
        if not quiet:
            gcmd.respond_info('IMONK committed view')

    def cmd_IMONK_ABORT_VIEW(self, gcmd: GCodeCommand):
        quiet = gcmd.get_int('QUIET', 0) == 1

        sid = gcmd.get_int('SID')
        self.api.view_abort(sid)
        if not quiet:
            gcmd.respond_info('IMONK aborted view setup')

    def cmd_IMONK_REBOOT(self, gcmd: GCodeCommand):
        quiet = gcmd.get_int('QUIET', 0) == 1

        self.api.reboot()
        if not quiet:
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

        images = self.api.images_manifest()
        if not quiet:
            gcmd.respond_info(f'IMONK Images: {images}')
        views = self.api.views_manifest()
        if not quiet:
            gcmd.respond_info(f'IMONK Views: {views}')
            gcmd.respond_info('IMONK State loaded!' + self._diff_message())

    def cmd_IMONK_SYNCHRONIZE(self, gcmd: GCodeCommand):
        quiet = gcmd.get_int('QUIET', 0) == 1

        if self._is_printing():
            raise gcmd.error('Cannot synchronize IMONK while printing')

        known_images = set(self.state.host.images.keys())

        for view in self.state.host.views.values():
            diff = view.used_images.difference(known_images)
            if diff:
                raise gcmd.error(f'IMONK View {view.name} references not existing images {diff}')

        for name in self.state.device.images.keys():
            if name not in self.state.host.images:
                self.api.remove_image(name)
                if not quiet:
                    gcmd.respond_info(f'IMONK Removed unused image {name} from device')

        for name in self.state.device.views.keys():
            if name not in self.state.host.views:
                self.api.remove_view(name)
                if not quiet:
                    gcmd.respond_info(f'IMONK Removed unused view {name} from device')

        for name, resource in self.state.host.views.items():
            dev_crc = self.state.device.views.get(name, -1)
            if dev_crc != resource.crc:
                if not quiet:
                    gcmd.respond_info(f'IMONK View CRC mismatch Host {resource.crc}, Device {dev_crc}')
                self.api.upload_view(name, resource.data)
                if not quiet:
                    if dev_crc == -1:
                        gcmd.respond_info(f'IMONK Uploaded view {name}')
                    else:
                        gcmd.respond_info(f'IMONK Updated view {name}')

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
        views = {}
        for section in config.get_prefix_sections('imonk '):
            resource = build_resource(section)
            if isinstance(resource, IMONKResourceImage):
                images[resource.name] = resource
            elif isinstance(resource, IMONKResourceView):
                views[resource.name] = resource
            else:
                raise ValueError(f'Unknown resource type {type(resource)}')

        self.state.host.images = images
        self.state.host.views = views

    def _is_printing(self) -> bool:
        return self.idle_timeout.state == "Printing"

    def _diff_message(self) -> str:
        diff_images = self.state.host.images != self.state.device.images
        diff_views = self.state.host.views != self.state.device.views

        rest = "seem to differ between Klipper and the device, use IMONK_SYNCHRONIZE to get them back in sync"
        if diff_images and diff_views:
            return "\nImages and Views " + rest
        if diff_images:
            return "\nImages " + rest
        if diff_views:
            return "\nViews " + rest

        return ""

    def _parse_value(self, value):
        try:
            value = literal_eval(value)
        except (ValueError, SyntaxError):
            pass
        return value

    def _get_view_state(self, view):
        return (view[0], view[1].name) if view is not None else None

    def _stage_if_needed(self, name: str) -> tuple[int, bool]:
        if self.state.view.current and self.state.view.current[1].name == name:
            return self.state.view.current[0], True

        if self.state.view.staged and self.state.view.staged[1].name == name:
            return self.state.view.staged[0], False

        return self.api.view_stage(name), False

    def _set_view(self, name: str, data: dict):
        view_id, committed = self._stage_if_needed(name)

        for key, value in data.items():
            self.api.view_set_value(view_id, key, value)

        if not committed:
            self.api.view_commit(view_id)

        return view_id
