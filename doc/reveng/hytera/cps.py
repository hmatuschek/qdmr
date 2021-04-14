#!/usr/bin/env python3
import struct
import argparse
from prettytable import PrettyTable  # type: ignore
import packet
from radio import CPSRadio
from typing import List
from enum import Enum


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

    @staticmethod
    def size() -> int:
        """Return the size of the object."""
        return struct.calcsize(DigitalChannel.STRUCT_FMT)


def read(radio: CPSRadio, start_addr: int, length: int) -> bytes:
    """Read a contiguous memory range from CPS memory."""
    read_chunk_sz = 0x100
    end_address = start_addr + length
    ret = bytes()

    for addr in range(start_addr, end_address, read_chunk_sz):
        length = read_chunk_sz if (
            addr + read_chunk_sz) < end_address else end_address - addr

        resp = radio.xfer(packet.Packet.readCodeplug(addr, length))

        if isinstance(resp._content, packet.EmptyContent):
            continue

        ret += resp._content._content._payload

    return ret


def dump_cp_memory(args) -> None:
    with CPSRadio(args.verbose) as radio:
        data = read(radio, 0, 0x200000)
        args.OUT_FILE.write(data)


def read_digital_contacts(radio: CPSRadio) -> List[DigitalContact]:
    num_contacts = struct.unpack('<H', read(radio, 0x65b5c, 2))[0]
    contact_sz = DigitalContact.size()
    table_sz = num_contacts * contact_sz

    start_addr = 0x65b70
    stop_addr = start_addr + table_sz

    contacts: List[DigitalContact] = []

    for contact_addr in range(start_addr, stop_addr, contact_sz):
        contacts.append(DigitalContact(read(radio, contact_addr, contact_sz)))

    return contacts


def read_digital_channels(radio: CPSRadio) -> List[DigitalChannel]:
    table_elements = struct.unpack('<H', read(radio, 0x3ab18, 2))[0]
    element_sz = DigitalChannel.size()
    start_addr = 0x3ab2a

    channels: List[DigitalChannel] = []

    for i in range(start_addr, start_addr + (table_elements * element_sz),
                   element_sz):
        data = read(radio, i, element_sz)

        channels.append(DigitalChannel(data))

    return channels


def dump_digital_channels(args) -> None:
    with CPSRadio(args.verbose) as radio:
        contacts = read_digital_contacts(radio)
        channels = read_digital_channels(radio)

        table = PrettyTable(['Name', 'Tx Freqneucy', 'Rx Frequency', 'Contact',
                             'Slot', 'Colour Code', 'Power', 'Scan', 'RO', 'TOT',
                             'Admit', 'RxGL'])

        for channel in channels:
            table.add_row([channel.name, channel.tx_freq, channel.rx_freq,
                           contacts[channel.tx_contact_idx - 1].name,
                           channel.timeslot + 1, channel.colour_code,
                           channel.power, channel.scan_list_idx,
                           channel.rx_only, channel.tx_timeout,
                           channel.tx_admit, channel.rx_group_list_idx])

        print(table)


def dump_contacts(args) -> None:
    with CPSRadio(args.verbose) as radio:
        contacts = read_digital_contacts(radio)

        table = PrettyTable([
            'Index', 'Name', 'Type', 'Is referenced', 'id', 'link'])

        for contact in contacts:
            if contact.call_type != DigitalContact.CALL_TYPE.IGNORE:
                table.add_row([contact.idx, contact.name, contact.call_type,
                               contact.is_ref, contact.id, contact.link])

        print(table)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Communicate with Hytera devices in CPS mode")
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="Enable verbose mode (shows all packets sent and received).")

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

    dumpDigitalChannelParser = subparsers.add_parser(
        "dumpDigitalChannels", help="Dump digitial channels")
    parsedArgs = parser.parse_args()

    if parsedArgs.command == "readCP":
        dump_cp_memory(parsedArgs)
    elif parsedArgs.command == "dumpContacts":
        dump_contacts(parsedArgs)
    elif parsedArgs.command == "dumpDigitalChannels":
        dump_digital_channels(parsedArgs)
