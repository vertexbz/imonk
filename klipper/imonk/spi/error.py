from __future__ import annotations


class CommunicationError(Exception):
    ...


class CRCError(CommunicationError):
    def __init__(self, crc_received: int, crc_computed: int):
        super().__init__(f'Communication failed, CRC Received {hex(crc_received)[2:].upper()} / CRC Computed {hex(crc_computed)[2:].upper()}')
