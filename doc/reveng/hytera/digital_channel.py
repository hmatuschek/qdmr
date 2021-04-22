import struct
from enum import Enum
from typing import Dict
from section import CPSSection
from util import chunks
from contact import parse_digital_contacts
from prettytable import PrettyTable  # type: ignore


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


def parse_digital_channels(cps: Dict[int, CPSSection]) -> \
        Dict[int, DigitalChannel]:

    section = cps.get(0x26)

    if section is None:
        raise RuntimeError("No digital channel section found in codeplug.")

    num_channels = section.header.no_elements
    channel_sz = DigitalChannel.size()

    channels = []

    for data in chunks(section.data, channel_sz)[:num_channels]:
        channels.append(DigitalChannel(data))

    ret = dict()

    # Calculate mappings
    for i, idx in enumerate(section.mappings[:len(channels)]):
        ret[i] = channels[idx]

    return ret


def print_digital_channels(cps: Dict[int, CPSSection]) -> None:
    contacts = parse_digital_contacts(cps)
    channels = parse_digital_channels(cps)

    headers = ['Name', 'Tx Freqneucy', 'Rx Frequency', 'Contact',
               'Slot', 'Colour Code', 'Power', 'Scan', 'RO', 'TOT',
               'Admit', 'RxGL']

    table = PrettyTable(headers)

    for channel in channels.values():
        row = [channel.name, channel.tx_freq, channel.rx_freq,
               contacts[channel.tx_contact_idx - 1].name,
               channel.timeslot + 1, channel.colour_code,
               channel.power, channel.scan_list_idx,
               channel.rx_only, channel.tx_timeout,
               channel.tx_admit, channel.rx_group_list_idx]

        table.add_row(row)

    print(table)
