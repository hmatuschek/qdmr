import struct
from typing import Any


class ChannelPointer:
    """Represents a pointer from to a channel in a zone list."""
    STRUCT_FMT = "<HH"

    def __init__(self, data: bytes) -> None:
        fields = struct.unpack(ChannelPointer.STRUCT_FMT, data)
        self.addr = fields[0]

        assert self.addr != 0

        assert fields[1] <= 1

        self.is_analog = bool(fields[1])

    def __eq__(self, o: Any) -> bool:
        if not isinstance(o, ChannelPointer):
            return False

        return (o.addr == self.addr and
                o.is_analog == self.is_analog)

    @staticmethod
    def size() -> int:
        """Return the size of the element in the codeplug."""
        return struct.calcsize(ChannelPointer.STRUCT_FMT)
