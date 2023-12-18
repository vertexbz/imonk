from __future__ import annotations
from typing import TYPE_CHECKING
from .image import IMONKResourceImage
from .scene import IMONKResourceScene

if TYPE_CHECKING:
    from configfile import ConfigWrapper


def build_resource(config: ConfigWrapper):
    _, kind, name = config.get_name().split(' ', maxsplit=3)
    kind = kind.lower()

    if ' ' in name:
        raise config.error(f'Invalid IMONK configuration entry, invalid resource name {name}')

    if 'image' == kind:
        return IMONKResourceImage(config, name)
    if 'scene' == kind:
        return IMONKResourceScene(config, name)

    raise config.error(f'Invalid IMONK configuration entry, unknown resource type {kind}')


__all__ = ['IMONKResourceScene', 'IMONKResourceImage', 'build_resource']
