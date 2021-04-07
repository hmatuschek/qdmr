#!/usr/bin/env python3

import usb.core
import usb.util
import binascii
import argparse

import packet
import struct

isVerbose = False

class Radio:
    READ_SZ = 0x400
    TIMEOUT = 1500
    OUT_EP = 0x01
    IN_EP = 0x81

    def __init__(self):
        pass

    def __enter__(self):
        self._dev = usb.core.find(idVendor=0x8765, idProduct=0x1234)

        if self._dev is None:
            raise ValueError('Radio not found')

        self._dev.set_configuration()

        response = self.xfer(packet.FirmwarePacket.setMemoryAccess(True))
        openStatus = response._content._content._status

        if openStatus != packet.FwMemoryAccessResponse.STATUS_OK:
            raise RuntimeError("Failed to enable write on radio (status={:02X})".format(openStatus))

        return self

    def __exit__(self,exc_type, exc_value, traceback):
        response = self.xfer(packet.FirmwarePacket.setMemoryAccess(False))
        openStatus = response._content._content._status

        if openStatus != packet.FwMemoryAccessResponse.STATUS_OK:
            raise RuntimeError("Failed to disable write on radio (status={:02X})".format(openStatus))

    def xfer(self, cmd):
        if isVerbose:
            print('-'*80)
            print(cmd)

        self._dev.write(Radio.OUT_EP, cmd.pack(), Radio.TIMEOUT)
        data = bytes(self._dev.read(Radio.IN_EP, Radio.READ_SZ, Radio.TIMEOUT))


        dataSz = struct.unpack('<H', data[:2])[0]

        while len(data) != dataSz:
            data += bytes(self._dev.read(Radio.IN_EP, Radio.READ_SZ, Radio.TIMEOUT))

        pkt = packet.FirmwarePacket.unpack(data)

        if isVerbose:
            print(pkt)

        return pkt

def dump(outFile):
    with Radio() as radio:
        print("Reading...")
        for x in range(0, 0x1fffff, 0x100):
            resp = radio.xfer(packet.FirmwarePacket.readCodeplug(x, 0x100))
            status = resp._content._content._status
            if status != packet.FwReadCodeplugResponse.STATUS_SUCCESS:
                raise RuntimeError("Error reading CP memory (status {:02X})".format(status))
            data = resp._content._content._payload
            outFile.write(data)



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Communicate with Hytera devices in Firmware mode")
    parser.add_argument("-v", "--verbose", action="store_true",
                        help="Enable verbose mode (shows all packets sent and received).")

    subparsers = parser.add_subparsers(title="command",
                                       dest="command",
                                       required=True,
                                       description="Commands that can be sent to the radio")

    dumpCPMemoryParser = subparsers.add_parser("dump",
                                               help="Dump codeplug memory.")

    dumpCPMemoryParser.add_argument("OUT_FILE",
                                    type=argparse.FileType('bw'))

    args = parser.parse_args()

    isVerbose = args.verbose

    if args.command == "dump":
        dump(args.OUT_FILE)
