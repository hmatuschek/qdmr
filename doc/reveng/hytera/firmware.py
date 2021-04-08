#!/usr/bin/env python3

import usb.core
import usb.util
import binascii
import argparse
import packet
import struct
import os
from radio import FirmwareRadio

def dump(args):
    with FirmwareRadio(args.verbose) as radio:
        print("Reading...")
        for x in range(0, 0x1fffff, 0x100):
            resp = radio.xfer(packet.FirmwarePacket.readCodeplug(x, 0x100))
            status = resp._content._content._status
            if status != packet.FwReadMemoryResponse.STATUS_SUCCESS:
                raise RuntimeError("Error reading CP memory (status {:02X})".format(status))
            data = resp._content._content._payload
            args.OUT_FILE.write(data)


def write(args):
    data = args.CP_IMAGE.read()

    def chunks(lst, n):
        for i in range(0, len(lst), n):
            yield lst[i:i + n]

    addr = 0

    with FirmwareRadio(args.verbose) as radio:
        print("Writing...")
        for chunk in chunks(data, 0x100):
            resp = radio.xfer(packet.FirmwarePacket.writeCodePlug(addr, chunk))
            status = resp._content._content._status

            if status != packet.FwWriteMemoryResponse.STATUS_SUCCESS:
                raise RuntimeError("Error writing CP memory (status {:02X})".format(status))

            addr += len(chunk)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Communicate with Hytera devices in Firmware mode")
    parser.add_argument("-v", "--verbose", action="store_true",
                        help="Enable verbose mode (shows all packets sent and received).")

    subparsers = parser.add_subparsers(title="command",
                                       dest="command",
                                       required=True,
                                       description="Commands that can be sent to the radio")

    dumpCPMemoryParser = subparsers.add_parser("readCP",
                                               help="Read codeplug memory to file.")

    dumpCPMemoryParser.add_argument("OUT_FILE",
                                    type=argparse.FileType('bw'),
                                    help="Output file where codeplug memory is written.")

    writeCPMemoryParser = subparsers.add_parser("writeCP",
                                               help="Write codeplug memory.")

    writeCPMemoryParser.add_argument("CP_IMAGE",
                                     type=argparse.FileType('br'),
                                     help="Input file where CP memory is read from")

    args = parser.parse_args()

    if args.command == "readCP":
        dump(args)
    elif args.command == "writeCP":
        write(args)
