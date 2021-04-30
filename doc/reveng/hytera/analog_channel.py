from __future__ import annotations
import struct
from typing import Dict
from util import chunks
from section import CPSSection
from enum import Enum
from prettytable import PrettyTable  # type: ignore


class AnalogChannel:
    """Represents an analog channel in the codeplug."""
    # pylint: disable=too-many-instance-attributes
    # This class will have many attributes since it represnts a
    # digitial channel.
    STRUCT_FMT = '<32s4BII32B'

    class CTCSSTailRevertPhase(Enum):
        RAD_120 = 0
        RAD_180 = 1

        @staticmethod
        def unpack(data: int) -> AnalogChannel.CTCSSTailRevertPhase:
            return AnalogChannel.CTCSSTailRevertPhase((data & 0x2) >> 1)

    class CTCSSCDCSSType(Enum):
        NONE = 0
        CTCSS = 1
        CDCSS = 2
        CDCSS_INVERT = 3

        @staticmethod
        def unpack(data: int) -> AnalogChannel.CTCSSCDCSSType:
            return AnalogChannel.CTCSSCDCSSType(data >> 6)

    class SignalingType(Enum):
        NONE = 0
        TWO_TONE = 1

    class SQLMode(Enum):
        CARRIER = 0
        CTCSS_CDCSS = 1
        OPT_SIGNALING = 2
        CTCSS_CDCSS_AND_OPT_SIG = 3
        CTCSS_CDCSS_OR_OPT_SIG = 4

    class ChannelChangeSQLMode(Enum):
        RX_SQL_MODE = 0
        MONITOR_SQL_MODE = 1

    class CarrierSQLLevel(Enum):
        OPEN = 0
        NORMAL = 1
        TIGHT = 2

    class AutoResetMode(Enum):
        DISABLE = 0
        CARRIER_OVERRIDE = 1
        CARRIER_INDEPENDENT = 2
        MANUAL_OVERRIDE = 3

    class PowerLevel(Enum):
        HIGH = 0
        LOW = 1

        @staticmethod
        def unpack(data: int) -> AnalogChannel.PowerLevel:
            return AnalogChannel.PowerLevel((data & 0x4) >> 2)

    class TXAdmit(Enum):
        ALWAYS = 0
        CHANNEL_FREE = 1
        CTCSS_CORRECT = 2
        CTCSS_INCORRECT = 3

    def __init__(self, data: bytes) -> None:
        fields = struct.unpack(AnalogChannel.STRUCT_FMT, data)
        self.name = fields[0].decode('utf-8').replace('\x00', '')
        self.rx_only = bool(fields[2] & 0x1)
        self.power_level = AnalogChannel.PowerLevel.unpack(fields[2])
        self.rx_freq = fields[5]
        self.tx_freq = fields[6]
        self.rx_ctcss_type = AnalogChannel.CTCSSCDCSSType.unpack(fields[8])
        self.rx_ctcss_cdcss = ((fields[8] & 0xf) << 8) + fields[7]
        self.tx_ctcss_type = AnalogChannel.CTCSSCDCSSType.unpack(fields[10])
        self.tx_ctcss_cdcss = ((fields[10] & 0xf) << 8) + fields[9]
        self.rx_sql_mode = AnalogChannel.SQLMode(fields[11])
        self.mon_sql_mode = AnalogChannel.SQLMode(fields[12])
        self.channel_change_sql_mode = AnalogChannel.ChannelChangeSQLMode(fields[13])
        self.carrier_sql_level = AnalogChannel.CarrierSQLLevel(fields[15])
        self.tx_admit = AnalogChannel.TXAdmit(fields[16])
        self.tx_timeout = fields[17]
        self.tot_prealert = fields[18]
        self.tot_rekey = fields[19]
        self.tot_reset = fields[20]
        self.auto_reset_mode = AnalogChannel.AutoResetMode(fields[21])
        self.auto_reset_time = fields[22]
        self.signal_type = AnalogChannel.SignalingType(fields[24])
        self.emergency_system_idx = fields[26]
        self.emergency_alarm_indication = bool(fields[27] & 0x1)
        self.emergency_alarm_ack = bool(fields[27] & 0x2)
        self.emergency_call_indication = bool(fields[27] & 0x4)
        self.scan_list_idx = fields[28]
        self.auto_start_scan = bool(fields[29] & 0x1)
        self.emph_de_emph = bool(fields[29] & 0x10)
        self.scrambler = bool(fields[29] & 0x40)
        self.compandor = bool(fields[29] & 0x20)
        self.vox = bool(fields[30] & 0x80)
        self.ctcss_tail_revert = AnalogChannel.CTCSSTailRevertPhase.unpack(fields[30])
        self.option_board = bool(fields[32] & 0x04)
        self.tail_revert = bool(fields[32] & 0x08)
        self.ctcss_required_for_data = bool(fields[30] & 0x40)
        self.phone_system_idx = fields[35]
        self.rssi_threshold = -fields[37]

    @staticmethod
    def size() -> int:
        """Return the size of the object."""
        return struct.calcsize(AnalogChannel.STRUCT_FMT)


def parse_analog_channels(cps: Dict[int, CPSSection]) -> Dict[int, AnalogChannel]:
    section = cps.get(0x27)

    if section is None:
        raise RuntimeError("No analog channel section found in codeplug.")

    num_channels = section.header.no_elements
    channel_sz = AnalogChannel.size()

    channels = []

    for data in chunks(section.data, channel_sz)[:num_channels]:
        channels.append(AnalogChannel(data))

    ret = dict()

    # Calculate mappings
    for i, idx in enumerate(section.mappings[:len(channels)]):
        ret[i] = channels[idx]

    return ret


def print_analog_channels(cps: Dict[int, CPSSection]) -> None:
    channels = parse_analog_channels(cps)

    headers = ['Name', 'Tx Freqneucy', 'Rx Frequency',
               'Power', 'Scan', 'RO', 'TOT',
               'Admit', 'Squelch', 'RX Tone', 'TX Tone']

    table = PrettyTable(headers)

    for channel in channels.values():
        rx_ctcss = '-'
        tx_ctcss = '-'

        if channel.rx_ctcss_type == AnalogChannel.CTCSSCDCSSType.CTCSS:
            rx_ctcss = channel.rx_ctcss_cdcss / 10

        if channel.tx_ctcss_type == AnalogChannel.CTCSSCDCSSType.CTCSS:
            tx_ctcss = channel.tx_ctcss_cdcss / 10

        row = [channel.name,
               channel.tx_freq / 1000000,
               channel.rx_freq / 1000000,
               channel.power_level,
               channel.scan_list_idx,
               channel.rx_only,
               channel.tx_timeout,
               channel.tx_admit,
               channel.carrier_sql_level,
               rx_ctcss,
               tx_ctcss]
        table.add_row(row)

    print(table)
