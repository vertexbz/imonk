from asyncio import Lock
from tornado.web import stream_request_body, HTTPError
from tornado.http1connection import HTTP1Connection
from streaming_form_data import StreamingFormDataParser, ParseFailedException
from streaming_form_data.targets import FileTarget, ValueTarget, SHA256Target
from typing import Callable, Optional
import logging
import os
from ..file_manager.file_manager import FileManager
from ...app import AuthorizedRequestHandler
from ...utils import json_wrapper as jsonw, ServerError


@stream_request_body
class UploadHandler(AuthorizedRequestHandler):
    def initialize(self, endpoint: str = '', max_upload_size: int = 0, handler: Optional[Callable] = None) -> None:
        self.max_upload_size = max_upload_size
        self.handler = handler
        self.server = self.settings['server']
        self.endpoint = endpoint
        self.location_prefix = endpoint
        self.file_manager: FileManager = self.server.lookup_component('file_manager')
        self.parse_lock = Lock()
        self.parse_failed: bool = False

    def prepare(self) -> None:
        super(UploadHandler, self).prepare()
        content_type: str = self.request.headers.get('Content-Type', '')
        logging.info(
            f'Upload Request Received from {self.request.remote_ip}\n'
            f'Content-Type: {content_type}'
        )
        self.file_manager.check_write_enabled()
        if self.request.method == 'POST':
            assert isinstance(self.request.connection, HTTP1Connection)
            self.request.connection.set_max_body_size(self.max_upload_size)
            tmpname = self.file_manager.gen_temp_upload_path()
            self._file = FileTarget(tmpname)
            self._parser = StreamingFormDataParser(self.request.headers)
            self._parser.register('file', self._file)

    async def data_received(self, chunk: bytes) -> None:
        if self.request.method == 'POST' and not self.parse_failed:
            async with self.parse_lock:
                evt_loop = self.server.get_event_loop()
                try:
                    await evt_loop.run_in_thread(self._parser.data_received, chunk)
                except ParseFailedException:
                    logging.exception('Chunk Parsing Error')
                    self.parse_failed = True

    async def post(self) -> None:
        if self.parse_failed:
            self._file.on_finish()
            self._cleanup(self._file)
            raise HTTPError(500, 'File Upload Parsing Failed')

        try:
            result = await self.handler(tmp_file_path=self._file.filename, filename=self._file.multipart_filename)
        except ServerError as e:
            raise HTTPError(e.status_code, str(e))
        finally:
            self._cleanup(self._file)

        self.set_status(200)
        self.set_header('Content-Type', 'application/json; charset=UTF-8')
        self.finish(jsonw.dumps({'result': result}))

    def _cleanup(self, file: FileTarget):
        try:
            os.remove(file.filename)
        except Exception:
            pass
