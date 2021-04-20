#!/usr/bin/env python3
from __future__ import annotations
import struct
import argparse
from prettytable import PrettyTable  # type: ignore
import packet
from radio import CPSRadio
from typing import List, BinaryIO, Dict
from enum import Enum
from abc import ABC, abstractmethod


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


class DigitalContact:
    """A class that represents a digitial channel."""
    STRUCT_FMT = '<H32sBBHIIH'

    class CALL_TYPE(Enum):
        """Type of call that is made when this contact is called.

        If the contact type is IGNORE, the contact is ignored by the radio and
        the CPS.
        """
        PRIVATE = 0
        GROUP = 1
        IGNORE = 0x11

    def __init__(self, data: bytes) -> None:
        fields = struct.unpack(DigitalContact.STRUCT_FMT, data)
        self.idx = fields[0]
        self.name = fields[1].decode('utf-8')
        self.call_type = DigitalContact.CALL_TYPE(fields[2])
        self.is_ref = bool(fields[3])
        self.id = fields[5]
        self.link = fields[7]

    @staticmethod
    def size() -> int:
        """Get the size of the contact structure."""
        return struct.calcsize(DigitalContact.STRUCT_FMT)


class ZoneName:
    """Represents a zone name in the codeplug."""
    STRUCT_FMT = '<32sH6B'

    def __init__(self, data: bytes) -> None:
        fields = struct.unpack(ZoneName.STRUCT_FMT, data)
        self.name = fields[0].decode('utf-8')
        self.num_channels = fields[1]
        self.list_byte_check = fields[2:]

    @staticmethod
    def size() -> int:
        """Return the size of the element in the codeplug."""
        return struct.calcsize(ZoneName.STRUCT_FMT)


class ZonePointer:
    """Represents a pointer from to a channel in a zone list."""
    STRUCT_FMT = "<HH"

    def __init__(self, data: bytes) -> None:
        fields = struct.unpack(ZonePointer.STRUCT_FMT, data)
        self.addr = fields[0]

        assert self.addr != 0

        assert fields[1] <= 1

        self.is_analog = bool(fields[1])

    @staticmethod
    def size() -> int:
        """Return the size of the element in the codeplug."""
        return struct.calcsize(ZonePointer.STRUCT_FMT)

    def __eq__(self, o: ZonePointer):
        return o.addr == self.addr and o.is_analog == self.is_analog


class DigitalChannel:
    """Represents a digitial channel in the codeplug."""
    # pylint: disable=too-many-instance-attributes
    # This class will have many attributes since it represnts a
    # digitial channel.
    STRUCT_FMT = '<32sbbHIIbbbbbbHHHbbbbbbHbbHbbbbH10b'

    class PowerLevel(Enum):
        """The default power level for the channel."""
        HIGH = 0x0
        LOW = 0x4

    class TxAdmit(Enum):
        """The TX admit mode for the channel."""
        ALWAYS = 0
        CHANNEL = 1
        COLOR_CODE = 2

    def __init__(self, data: bytes) -> None:
        fields = struct.unpack(DigitalChannel.STRUCT_FMT, data)
        self.name = fields[0].decode('utf-8')
        self.rx_only = bool(fields[2] & 0x01)
        self.power = DigitalChannel.PowerLevel(fields[2] & 0x04)
        self.rx_freq = fields[4]
        self.tx_freq = fields[5]
        self.tx_admit = DigitalChannel.TxAdmit(fields[6])
        self.tx_timeout = fields[7]
        self.tx_timeout_prealert = fields[8]
        self.tx_timeout_rekey = fields[9]
        self.tx_timeout_reset = fields[10]
        self.colour_code = fields[11] & 0x0f
        self.priority_interrupt_encode = fields[11] & 0x20
        self.priority_interrupt_decode = fields[11] & 0x40
        self.tx_contact_idx = fields[12]
        self.rx_group_list_idx = fields[13]
        self.emergency_system_idx = fields[14]
        self.scan_list_idx = fields[15]
        self.timeslot = fields[17] & 0x3
        self.has_scan_idx = fields[17] & 0x10
        self.vox = fields[17] & 0x20
        self.option_board = fields[17] & 0x80
        self.loc_revert_ch_idx = fields[21]
        self.phone_system_idx = fields[24]
        self.pseudo_trunk_tx = fields[25]
        self.rrs_revert_ch = fields[29]
        self.unk1 = struct.unpack('<H', data[-4:-2])[0]
        self.unk2 = struct.unpack('<H', data[-2:])[0]

    @staticmethod
    def size() -> int:
        """Return the size of the object."""
        return struct.calcsize(DigitalChannel.STRUCT_FMT)


def dump_cp_memory(reader: CPReader, args) -> None:
    """Do a raw dump of all CP memory."""
    data = reader.read(0, 0x200000)
    args.OUT_FILE.write(data)


def read_digital_contacts(reader: CPReader) -> List[DigitalContact]:
    num_contacts = struct.unpack('<H', reader.read(0x65b5c, 2))[0]
    contact_sz = DigitalContact.size()
    table_sz = num_contacts * contact_sz

    start_addr = 0x65b70
    stop_addr = start_addr + table_sz

    contacts: List[DigitalContact] = []

    for contact_addr in range(start_addr, stop_addr, contact_sz):
        contacts.append(DigitalContact(reader.read(contact_addr, contact_sz)))

    return contacts


def read_digital_channel_mapping(reader: CPReader) -> Dict[int, int]:
    start_addr = 0x0004fad6
    mappings = []
    STRUCT_FMT = '<HI'
    sz = struct.calcsize(STRUCT_FMT)

    for i, addr in enumerate(range(start_addr,
                                   start_addr + (0x3ff * sz),
                                   sz)):
        idx, offset = struct.unpack('<HI', reader.read(addr, 6))
        assert offset // DigitalChannel.size() == idx
        mappings.append((i, idx))

    return dict(mappings)


def read_zone_names(reader: CPReader) -> List[ZoneName]:
    element_sz = ZoneName.size()
    start_addr = 0x39764
    table_elements = struct.unpack('<H', reader.read(start_addr - 0x12, 2))[0]

    zone_names: List[ZoneName] = []

    for i in range(start_addr, start_addr + (table_elements * element_sz),
                   element_sz):
        data = reader.read(i, element_sz)

        zone_names.append(ZoneName(data))

    return zone_names


def read_digital_channels(reader: CPReader) -> List[DigitalChannel]:
    table_elements = struct.unpack('<H', reader.read(0x3ab18, 2))[0]
    element_sz = DigitalChannel.size()
    start_addr = 0x3ab2a

    channels: List[DigitalChannel] = []

    for i in range(start_addr, start_addr + (table_elements * element_sz),
                   element_sz):
        data = reader.read(i, element_sz)

        channels.append(DigitalChannel(data))

    return channels


def read_zone_list(reader: CPReader,
                   idx: int, zone: ZoneName) -> List[ZonePointer]:
    """Read a zone list portion of CP memory.

    Parameters:
      - radio: The radio object used to do IO.
      - idx: The index of the zone.
      - zone_name: the ZoneName object.
    """
    start_addr = 0x125c + (idx * 0xe0e)
    element_sz = ZonePointer.size()
    table_elements = zone.num_channels * 2 - 1
    zone_pointers = []

    # Each zone name entry appears to contain the first six bytes of the zone
    # list data. Presumably this is some kind of anti-corruption check? Ensure
    # that is the case.
    beginningBytes = reader.read(start_addr, 6)
    assert bytes(beginningBytes) == bytes(zone.list_byte_check)

    for i in range(start_addr,
                   start_addr + (table_elements * element_sz),
                   element_sz):
        data = reader.read(i, element_sz)

        zone_pointers.append(ZonePointer(data))

    # Other than the first entry, all others appear to be duplicated. Check tha
    # assertion and only yield de-duped entries.
    ret = [zone_pointers[0]]
    for x in range(1, len(zone_pointers), 2):
        ret.append(zone_pointers[x])
        assert zone_pointers[x] == zone_pointers[x + 1]

    return ret


def dump_digital_channels(reader: CPReader) -> None:
    contacts = read_digital_contacts(reader)
    channels = read_digital_channels(reader)

    headers = ['Name', 'Tx Freqneucy', 'Rx Frequency', 'Contact',
               'Slot', 'Colour Code', 'Power', 'Scan', 'RO', 'TOT',
               'Admit', 'RxGL']

    table = PrettyTable(headers)

    for i, channel in enumerate(channels):
        row = [channel.name, channel.tx_freq, channel.rx_freq,
               contacts[channel.tx_contact_idx - 1].name,
               channel.timeslot + 1, channel.colour_code,
               channel.power, channel.scan_list_idx,
               channel.rx_only, channel.tx_timeout,
               channel.tx_admit, channel.rx_group_list_idx]

        table.add_row(row)

    print(table)


def dump_zones(reader: CPReader) -> None:
    channels = read_digital_channels(reader)
    zoneNames = read_zone_names(reader)
    mappings = read_digital_channel_mapping(reader)

    # TODO: print analog channels in zones.
    for i, zone in enumerate(zoneNames):
        zone_list = read_zone_list(reader, i, zone)

        print(zone.name)

        for zone_ptr in zone_list:
            if not zone_ptr.is_analog:
                channel = channels[mappings.get(zone_ptr.addr - 1)]
                print('  addr: {} ({:02X}) channel: {}'.format(zone_ptr.addr,
                                                               zone_ptr.addr,
                                                               channel.name))


def dump_contacts(reader: CPReader) -> None:
    contacts = read_digital_contacts(reader)

    table = PrettyTable([
        'Index', 'Name', 'Type', 'Is referenced', 'id', 'link'])

    for contact in contacts:
        if contact.call_type != DigitalContact.CALL_TYPE.IGNORE:
            table.add_row([contact.idx, contact.name, contact.call_type,
                           contact.is_ref, contact.id, contact.link])

    print(table)


def go(reader: CPReader, args):
    if args.command == "readCP":
        dump_cp_memory(reader, args)
    elif args.command == "dumpContacts":
        dump_contacts(reader)
    elif args.command == "dumpZones":
        dump_zones(reader)
    elif args.command == "dumpDigitalChannels":
        dump_digital_channels(reader)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Communicate with Hytera devices/CPSImages in CPS mode")
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="Enable verbose mode (shows all packets sent and received).")

    parser.add_argument(
        "-i",
        "--image",
        type=argparse.FileType('br'),
        help="Instead of reading from the radio, read from the provided codeplug image")

    subparsers = parser.add_subparsers(
        title="command",
        dest="command",
        required=True,
        description="Commands that can be sent to the radio")

    dumpCPMemoryParser = subparsers.add_parser(
        "readCP", help="Read codeplug memory to file.")

    dumpCPMemoryParser.add_argument(
        "OUT_FILE",
        type=argparse.FileType('bw'),
        help="Output file where codeplug memory is written.")

    dumpContactsParser = subparsers.add_parser(
        "dumpContacts", help="Read codeplug contacts and print to table.")

    dumpZonesParser = subparsers.add_parser(
        "dumpZones", help="Read codeplug zones and print to table.")

    dumpDigitalChannelParser = subparsers.add_parser(
        "dumpDigitalChannels", help="Dump digitial channels")

    parsedArgs = parser.parse_args()

    if parsedArgs.image is None:
        with CPSRadio(parsedArgs.verbose) as radio:
            go(CPSRadioReader(radio), parsedArgs)
    else:
        go(FileReader(parsedArgs.image), parsedArgs)
