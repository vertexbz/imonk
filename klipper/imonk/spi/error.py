from __future__ import annotations
from typing import Optional


def to_hex(byte: int) -> str:
    return hex(byte)[2:].upper().zfill(2)


class CommunicationError(Exception):
    def __init__(self, *a):
        self.position = dict()
        super().__init__(*a)

    def __str__(self):
        if self.position:
            posstr = str(self.position)[1:-1].replace("'", '')
            return super().__str__() + f' Position {posstr}'
        return super().__str__()


class CRCError(CommunicationError):
    def __init__(self, crc_computed: Optional[int] = None, crc_received: Optional[int] = None):
        msg = 'Invalid CRC.'
        if crc_computed is not None and crc_received is not None:
            msg = f'Communication failed, CRC Received {to_hex(crc_received)}'
            msg += f' / CRC Computed {to_hex(crc_computed)}.'
        elif crc_computed is not None:
            msg = f'Invalid CRC {to_hex(crc_computed)}.'

        super().__init__(msg)


class UnexpectedResponse(CommunicationError):
    def __init__(self, expected: int, received: int):
        super().__init__(f'Expected {to_hex(expected)}, but received {to_hex(received)}.')


class ByteCounter:
    def __init__(self, name: str, inner: Optional[ByteCounter] = None):
        self._inner = inner
        self._name = name
        self._n = 0

    def __enter__(self):
        self._n = 0
        return self

    def __exit__(self, exc_type: type, exc_val: CommunicationError, exc_tb):
        if exc_type is not None and issubclass(exc_type, CommunicationError):
            exc_val.position[self._name] = self._n

    def inc(self) -> None:
        if self._inner is not None:
            self._inner.inc()
        self._n = self._n + 1

    def value(self) -> int:
        return self._n

    def sub(self, name: str):
        return ByteCounter(name, self)
