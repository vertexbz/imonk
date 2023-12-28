from __future__ import annotations
from dataclasses import dataclass, field
from typing import TYPE_CHECKING, Optional

if TYPE_CHECKING:
    from .resource import IMONKResourceImage
    from .resource import IMONKResourceView


@dataclass
class ViewsState:
    current: Optional[tuple[int, IMONKResourceView]] = None
    staged: Optional[tuple[int, IMONKResourceView]] = None


@dataclass
class DeviceState:
    images: dict[str, int] = field(default_factory=dict)
    views: dict[str, int] = field(default_factory=dict)


@dataclass
class HostState:
    images: dict[str, IMONKResourceImage] = field(default_factory=dict)
    views: dict[str, IMONKResourceView] = field(default_factory=dict)


@dataclass(frozen=True)
class State:
    device: DeviceState = field(default_factory=DeviceState)
    host: HostState = field(default_factory=HostState)
    view: ViewsState = field(default_factory=ViewsState)
