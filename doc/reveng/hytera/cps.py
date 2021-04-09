#!/usr/bin/env python3

import sys
import binascii
import itertools
import usb.core
import usb.util
import math
import struct
from radio import CPSRadio
from prettytable import PrettyTable
from typing import List, Tuple
import argparse
import packet

def read(radio : CPSRadio, startAddr : int, length : int) -> bytes:
  read_chunk_sz = 0x100
  end_address = startAddr + length
  ret = bytes()

  for addr in range(startAddr, end_address, read_chunk_sz):
    length = read_chunk_sz if (addr + read_chunk_sz) < end_address else end_address - addr

    resp = radio.xfer(packet.Packet.readCodeplug(addr, length))

    ret += resp._content._content._payload

  return ret

def dump(args):
  with CPSRadio(args.verbose) as radio:
    data = read(radio, 0, 0x1fffff)
    args.OUT_FILE.write(data)

def dumpContacts(args):
  with CPSRadio(args.verbose) as radio:
    contactStructure = '<H32sBBHIIH'
    tableElements = struct.unpack('<H', read(radio, 0x65b5c, 2))[0]
    tableSize = tableElements * struct.calcsize(contactStructure)
    contactTable = read(radio, 0x65b70, tableSize)

    table = PrettyTable(['Index','Name', 'Type', 'Is referenced', 'pad1', 'id', 'unk1', 'link'])
    contactSlots = []

    # Build contact table.
    for idx, name, type, is_ref, pad1, id, unk1, link in struct.iter_unpack(contactStructure, contactTable):
      contactSlots.append([idx, name.decode('utf-8'), type, is_ref, pad1, id, unk1, link])

    # Now traverse
    for currentSlot in contactSlots:
      type = currentSlot[2]
      isRef = currentSlot[3]

      typeName = "Unknown"
      if type == 0:
        typeName = 'Private Call'
      elif type == 1:
        typeName = 'Group Call'

      isRefString = "Unknown"

      isRefBool = bool(isRef)

      currentSlot[2] = typeName
      currentSlot[3] = isRefBool

      if type != 0x11:
        table.add_row(currentSlot)

    print(table)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Communicate with Hytera devices in CPS mode")
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

    dumpContactsParser = subparsers.add_parser("readContacts",
                                               help="Read codeplug contacts and print to table.")

    args = parser.parse_args()

    if args.command == "readCP":
      dump(args)
    elif args.command == "readContacts":
      dumpContacts(args)
