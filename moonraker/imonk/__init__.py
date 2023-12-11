from __future__ import annotations
from typing import Dict, Any
from typing import TYPE_CHECKING
from base64 import b64encode
from ...common import WebRequest, Subscribable
from .upload_handler import UploadHandler

if TYPE_CHECKING:
    from klippy_connection import KlippyConnection as Klippy
    from server import Server
    from app import MoonrakerApp
    from confighelper import ConfigHelper


class IMONK(Subscribable):
    def __init__(self, config: ConfigHelper) -> None:
        self.server: Server = config.get_server()
        self.klippy: Klippy = self.server.lookup_component('klippy_connection')
        moonraker: MoonrakerApp = self.server.lookup_component('application')

        self.server.register_endpoint('/machine/imonk/version', ['GET'], self.handle_version)
        moonraker.mutable_router.add_handler('/machine/imonk/firmware', UploadHandler, {
            'max_upload_size': 2 * 1024 * 1024,
            'handler': self.handle_update
        })

    async def handle_version(self, _: WebRequest) -> Dict[str, Any]:
        return {
            'version': await self._request('imonk/version', {})
        }

    async def handle_update(self, tmp_file_path: str, **kw) -> Dict[str, Any]:
        with open(tmp_file_path, 'rb') as f:
            return await self._request('imonk/update', {
                'firmware': b64encode(f.read()).decode('ascii')
            })

    async def _request(self, endpoint: str, params: Dict[str, Any]) -> Dict[str, Any]:
        return await self.klippy.request(WebRequest(endpoint, params, conn=self))


def load_component(config: ConfigHelper) -> IMONK:
    return IMONK(config)
