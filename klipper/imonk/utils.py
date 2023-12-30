from __future__ import annotations
from typing import Callable
from typing import TYPE_CHECKING
import logging
from .klipper import CommandError
from .spi.error import CommunicationError

if TYPE_CHECKING:
    from .klipper import GCodeCommand


def with_error_handler(fn: Callable[[GCodeCommand], None]):
    def wrapped(gcmd: GCodeCommand):
        try:
            fn(gcmd)
        except ValueError as e:
            logging.exception(str(e))
            raise CommandError(str(e))
        except CommunicationError as e:
            logging.exception(str(e))
            raise CommandError(str(e))
        except Exception as e:
            logging.exception(str(e))
            raise e

    return wrapped


class FunctionSmuggler:
    def __init__(self, fn: Callable):
        self._fn = fn

    def __deepcopy__(self, memodict={}):
        return self

    def __call__(self, *args, **kwargs):
        return self._fn(*args, **kwargs)

    def __str__(self):
        return '<gcode macro helper function>'

    def __repr__(self):
        return self.__str__()


class SmugglingDict(dict):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def keys(self):
        return map(lambda t: t[0], filter(lambda t: not isinstance(t[1], FunctionSmuggler), super().items()))
