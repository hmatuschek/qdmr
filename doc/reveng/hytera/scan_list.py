import struct
from typing import Dict, List
from section import CPSSection
from util import chunks
from digital_channel import parse_digital_channels
from channel_pointer import ChannelPointer


class ScanListName:
    """Represents a scan list name in the codeplug."""
    STRUCT_FMT = '<32s24b'

    def __init__(self, data: bytes, pointers: List[ChannelPointer]) -> None:
        fields = struct.unpack(ScanListName.STRUCT_FMT, data)
        self.name = fields[0].decode('utf-8')
        self.unk = fields[1:]
        self.pointers = pointers

    @staticmethod
    def size() -> int:
        """Return the size of the element in the codeplug."""
        return struct.calcsize(ScanListName.STRUCT_FMT)


def parse_scan_lists(cps: Dict[int, CPSSection]) -> Dict[int, ScanListName]:
    sn_section = cps.get(0x6D)
    sp_section = cps.get(0x4D)

    if sn_section is None:
        raise RuntimeError("No scanlist name section found in codeplug.")

    if sp_section is None:
        raise RuntimeError("No scanlist pointer section found in clodeplug.")

    # There should be a list of scanlist pointers for every scanlist.
    assert sn_section.header.no_elements == sp_section.header.no_elements

    num_SLs = sn_section.header.no_elements

    def read_sn_list(data: bytes, idx: int) -> List[ChannelPointer]:
        start_addr = i * 0x14e

        header_data = struct.unpack('<HHHII', data[start_addr:start_addr+0xe])

        assert header_data[1] == 0x20
        assert header_data[3] == 0x80
        assert header_data[4] == 0x8e

        num_pointers = header_data[2]

        pointers = []

        for ptr_data in chunks(data[start_addr+0xe:start_addr+0xe +(num_pointers * ChannelPointer.size())],
                               ChannelPointer.size()):
            pointers.append(ChannelPointer(ptr_data))

        return pointers

    scanlist_names = []

    for i, data in enumerate(chunks(sn_section.data, ScanListName.size())[:num_SLs]):
        pointer_list = read_sn_list(sp_section.data, i)
        scan_list = ScanListName(data, pointer_list)
        scanlist_names.append(scan_list)

    ret = dict()

    # Calculate mappings
    for i, idx in enumerate(sn_section.mappings[:num_SLs]):
        ret[i] = scanlist_names[idx]

    return ret


def print_scanlists(cps: Dict[int, CPSSection]) -> None:
    scanlists = parse_scan_lists(cps)
    channels = parse_digital_channels(cps)

    for scanlist in scanlists.values():
        print("Scanlist: {}".format(scanlist.name))
        for pointer in scanlist.pointers:
            if pointer.addr == 0xffff:
                print("  <Selected>")
            elif pointer.is_analog:
                print("  addr: {:02X} Channel: <ANALOG>".format(pointer.addr))
            else:
                print("  addr: {:02X} channel:{}".format(pointer.addr,
                                                         channels[pointer.addr - 1].name))
