#!/usr/bin/env python3
import argparse
from reader import CPReader, FileReader, CPSRadioReader
from radio import CPSRadio
from section import read_codeplug_sections
from contact import print_contacts
from digital_channel import print_digital_channels
from analog_channel import print_analog_channels
from zone import print_zones
from scan_list import print_scanlists


def dump_cp_memory(reader: CPReader, args) -> None:
    """Do a raw dump of all CP memory."""
    data = reader.read(0, 0x200000)
    args.OUT_FILE.write(data)


def go(reader: CPReader, args):
    if args.command == "dumpCPMemory":
        dump_cp_memory(reader, args)
        return

    cp = read_codeplug_sections(reader)

    if args.command == "printContacts":
        print_contacts(cp)
    elif args.command == "printZones":
        print_zones(cp)
    elif args.command == "printDigitalChannels":
        print_digital_channels(cp)
    elif args.command == "printAnalogChannels":
        print_analog_channels(cp)
    elif args.command == "printScanLists":
        print_scanlists(cp)
    elif args.command == "printSections":
        for section in cp.values():
            print(section)


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
        help="Instead of reading from the radio, read from the "
             "provided codeplug image")

    subparsers = parser.add_subparsers(
        title="command",
        dest="command",
        required=True,
        description="Commands that can be sent to the radio")

    dumpCPMemoryParser = subparsers.add_parser(
        "dumpCPMemory", help="Read codeplug memory to file.")

    dumpCPMemoryParser.add_argument(
        "OUT_FILE",
        type=argparse.FileType('bw'),
        help="Output file where codeplug memory is written.")

    dumpContactsParser = subparsers.add_parser(
        "printContacts", help="Read codeplug contacts and print to table.")

    dumpSections = subparsers.add_parser(
        "printSections", help="Read codeplug sections and print.")

    dumpZonesParser = subparsers.add_parser(
        "printZones", help="Read codeplug zones and print to table.")

    scanListsParser = subparsers.add_parser(
        "printScanLists", help="Read codeplug scanlists and print.")

    dumpDigitalChannelParser = subparsers.add_parser(
        "printDigitalChannels", help="Dump digitial channels")

    printAnalogChannelParser = subparsers.add_parser(
        "printAnalogChannels", help="print analog channels")

    parsedArgs = parser.parse_args()

    if parsedArgs.image is None:
        with CPSRadio(parsedArgs.verbose) as radio:
            go(CPSRadioReader(radio), parsedArgs)
    else:
        go(FileReader(parsedArgs.image), parsedArgs)
