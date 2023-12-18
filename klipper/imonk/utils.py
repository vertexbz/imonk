from __future__ import annotations
from typing import Callable
from typing import TYPE_CHECKING
import traceback
from gcode import CommandError
from .spi.error import CommunicationError

if TYPE_CHECKING:
    from gcode import GCodeCommand


def with_error_handler(fn: Callable[[GCodeCommand], None]):
    def wrapped(gcmd: GCodeCommand):
        try:
            fn(gcmd)
        except ValueError as e:
            raise CommandError(str(e))
        except CommunicationError as e:
            raise CommandError(str(e) + '\n' + traceback.format_exc())

    return wrapped
