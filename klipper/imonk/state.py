from __future__ import annotations
from dataclasses import dataclass, field
from typing import TYPE_CHECKING, Optional

if TYPE_CHECKING:
    from .resource import IMONKResourceImage
    from .resource import IMONKResourceScene


@dataclass
class ScenesState:
    current: Optional[tuple[int, IMONKResourceScene]] = None
    staged: Optional[tuple[int, IMONKResourceScene]] = None


@dataclass
class DeviceState:
    images: dict[str, int] = field(default_factory=dict)
    scenes: dict[str, int] = field(default_factory=dict)


@dataclass
class HostState:
    images: dict[str, IMONKResourceImage] = field(default_factory=dict)
    scenes: dict[str, IMONKResourceScene] = field(default_factory=dict)


@dataclass(frozen=True)
class State:
    device: DeviceState = field(default_factory=DeviceState)
    host: HostState = field(default_factory=HostState)
    scene: ScenesState = field(default_factory=ScenesState)
