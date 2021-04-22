#!/usr/bin/env python3
import packet
from abc import ABC, abstractmethod
from radio import CPSRadio
from typing import BinaryIO


class CPReader(ABC):

    @abstractmethod
    def read(self, start_addr: int, length: int) -> bytes:
        ...


class CPSRadioReader(CPReader):
    def __init__(self, radio: CPSRadio):
        self._radio = radio

    def read(self, start_addr: int, length: int) -> bytes:
        """Read a contiguous memory range from CPS memory."""
        read_chunk_sz = 0x100
        end_address = start_addr + length
        ret = bytes()

        for addr in range(start_addr, end_address, read_chunk_sz):
            length = read_chunk_sz if (
                addr + read_chunk_sz) < end_address else end_address - addr

            resp = self._radio.xfer(packet.Packet.readCodeplug(addr, length))

            if isinstance(resp._content, packet.EmptyContent):
                continue

            ret += resp._content._content._payload

        return ret


class FileReader(CPReader):
    def __init__(self, f: BinaryIO) -> None:
        self._file = f

    def read(self, start_addr: int, length: int) -> bytes:
        self._file.seek(start_addr)

        return self._file.read(length)
