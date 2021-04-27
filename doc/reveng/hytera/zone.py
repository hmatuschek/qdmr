import struct
from typing import Dict, List
from section import CPSSection
from util import chunks
from digital_channel import parse_digital_channels
from channel_pointer import ChannelPointer


class ZoneName:
    """Represents a zone name in the codeplug."""
    STRUCT_FMT = '<32sH6B'

    def __init__(self, data: bytes) -> None:
        fields = struct.unpack(ZoneName.STRUCT_FMT, data)
        self.name = fields[0].decode('utf-8')
        self.num_channels = fields[1]
        self.list_byte_check = fields[2:]
        self.pointers: List[ChannelPointer] = []

    @staticmethod
    def size() -> int:
        """Return the size of the element in the codeplug."""
        return struct.calcsize(ZoneName.STRUCT_FMT)


def parse_zones(cps: Dict[int, CPSSection]) -> Dict[int, ZoneName]:
    zn_section = cps.get(0x24)
    zp_section = cps.get(0x23)

    if zn_section is None:
        raise RuntimeError("No zone names section found in codeplug.")

    if zp_section is None:
        raise RuntimeError("No zone pointer section found in clodeplug.")

    # There should be a list of channel pointers for every zone.
    assert zn_section.header.no_elements == zp_section.header.no_elements

    num_zones = zn_section.header.no_elements
    zone_name_sz = ZoneName.size()

    # There appears to be a 14-byte header at the begining of the zonelist data.
    # Strip that off.
    zp_data = zp_section.data[14:]

    def read_zp_list(zp_data: bytes, idx: int,
                     zone: ZoneName) -> List[ChannelPointer]:
        start_addr = i * 0xe0e

        # Each zone list pointer is duplicated in the list, apart from the first
        # one.
        num_elements = zone.num_channels * 2 - 1
        end_addr = start_addr + ChannelPointer.size() * num_elements
        data = zp_data[start_addr:end_addr]

        # Ensure that the bytes listed in the zone name match those in the zone
        # list.
        assert bytes(zone.list_byte_check) == data[0:len(zone.list_byte_check)]

        pointers = []

        for ptr_data in chunks(data, ChannelPointer.size()):
            pointers.append(ChannelPointer(ptr_data))

        # Other than the first entry, all others appear to be duplicated. Check tha
        # assertion and only yield de-duped entries.
        ret = [pointers[0]]
        for x in range(1, len(pointers), 2):
            assert pointers[x] == pointers[x + 1]
            ret.append(pointers[x])

        return ret

    zones = []

    for i, data in enumerate(chunks(zn_section.data,
                                    zone_name_sz)[:num_zones]):
        zone = ZoneName(data)
        zone.pointers = read_zp_list(zp_data, i, zone)
        zones.append(zone)

    ret = dict()
    # Calculate mappings
    for i, idx in enumerate(zn_section.mappings[:len(zones)]):
        ret[i] = zones[idx]

    return ret


def print_zones(cps: Dict[int, CPSSection]) -> None:
    zones = parse_zones(cps)
    channels = parse_digital_channels(cps)

    for zone in zones.values():
        print("Zone: {}".format(zone.name))

        for ptr in zone.pointers:
            if ptr.is_analog:
                print("  <ANALOG_CHANNEL>")
            else:
                print("  Addr: {} channel: {}".format(ptr.addr,
                                                      channels[ptr.addr - 1].name))
