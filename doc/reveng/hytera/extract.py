#!/usr/bin/env python3

import struct
import sys
import binascii
import itertools
import usb.core
import usb.util
import math
from prettytable import PrettyTable
from functools import partial

import argparse
parser = argparse.ArgumentParser()
parser.add_argument("command", help="""What to extract. Possible commands are: serial (dumps serial data in hex).
""")
args = parser.parse_args()

def download(start_address, size):
  end_address = start_address + size
  READ_SZ = 0x400
  TIMEOUT = 1500
  OUT_EP = 0x04
  IN_EP = 0x84

  ret = bytes()

  dev = usb.core.find(idVendor=0x238b, idProduct=0x0a11)

  def compose_read_command(address, length):
    x = 0x5959 - (address + length + 1)

    # Count the number of times this number has wrapped around 0
    wraps = abs(math.floor(x / 0xffff))
    crc = struct.pack('>H', ((x - wraps) & 0xffff))
    lengthBytes = struct.pack('<H', length)
    addressBytes = struct.pack('<I', address)

    return b'\x7e\x01\x00\x00\x20\x10\x00\x00\x00\x1f' + \
      bytes([crc[0]]) + b'\xa4\x02\xc7\x01\x0c\x00\x00\x00\x00\x01\x00\x00' + \
      addressBytes + lengthBytes + bytes([crc[1]]) + b'\x03'

  if dev is None:
    raise ValueError('Radio not found')

  dev.set_configuration()

  # Send preamble packets
  for pkt in [b'\x7e\x00\x00\xfe\x20\x10\x00\x00\x00\x0c\x60\xe5',
              b'\x7e\x01\x00\x00\x20\x10\x00\x00\x00\x14\x31\xd3\x02\x03\x02\x01\x00\x00\x2c\x03',
              b'\x7e\x01\x00\x00\x20\x10\x00\x00\x00\x24\x02\xf1\x02\xc5\x01\x11\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5b\x03',
              b'\x7e\x01\x00\x00\x20\x10\x00\x00\x00\x14\x41\xc3\x02\x01\x02\x01\x00\x12\x1c\x03']:
    dev.write(OUT_EP, pkt, TIMEOUT)
    dev.read(IN_EP, 1024, TIMEOUT)

  for addr in range(start_address, end_address, READ_SZ):
    length = READ_SZ if (addr + READ_SZ) < end_address else end_address - addr
    dev.write(OUT_EP, compose_read_command(addr, length), TIMEOUT)
    data = dev.read(IN_EP, 0x700, TIMEOUT)

    cmd, flag, src, des, unknown, length, payload = unpackLayer0Data(data)
    plen = len(payload)
    if (0x04 != cmd) or (7 > plen):
      raise ValueError('Expected response packet')

    # unpack layer 1
    u1, f1, what, u2, crc, crc_comp, end, plen, payload = unpackRequestResponse(payload)
    if 0xC7 != what:
      raise ValueError('Expected only read data')

    # unpack layer 2
    addr, length, payload = unpackReadWriteResponse(payload)

    ret += payload

  dev.write(OUT_EP, b'\x7e\x01\x00\x00\x20\x10\x00\x00\x00\x14\xf4\x0f\x02\xc6\x01\x01\x00\x00\x6a\x03')
  dev.read(IN_EP, 1024, TIMEOUT)

  return ret

def hexDump(s, prefix="", addr=0):
  N = len(s)
  Nb = N//16
  if (N%16): Nb += 1
  res = ""
  for j in range(Nb):
    a,b = j*16, min((j+1)*16,N)

    h = " ".join(map("{:02x}".format, s[a:b]))
    h += "   "*(16-(b-a))
    t = ""
    for i in range(a,b):
      c = s[i]
      if c>=0x20 and c<0x7f:
        t += chr(c)
      else:
        t += "."

    res += (prefix + "{:08X} ".format(addr+16*j) + h + " | " + t + "\n")
  return res[:-1]


def unpackLayer0Data(data):
  cmd, flag, src, des, unknown, length = struct.unpack(">xBxBBBHH", data[:10])
  payload = data[10:]
  return cmd, flag, src, des, unknown, length, payload

def unpackRequestResponse(payload):
  msb, u1, f1, what, u2, length = struct.unpack("<BBBBBH", payload[:7])
  lsb, end = struct.unpack("BB", payload[-2:])
  crc, a,b = 0x5ECF, 1, -2
  if len(payload)%2==0: b=-3
  for x in struct.iter_unpack("<H", payload[a:b]):
    crc -= x[0]
  if len(payload)%2==0: crc -= payload[-3]
  crc = crc%0xffff
  return u1, f1, what, u2, ((msb<<8)|lsb), crc, end, length, payload[7:-2]

def unpackReadWriteRequest(payload):
  ukn1, ukn2, ukn3, ukn4, addr, length = struct.unpack("<bHHbIH", payload[:12])
  content = payload[12:]
  return addr, length, content

def unpackReadWriteResponse(payload):
  addr, length = struct.unpack("<xxxxxxxIH", payload[:13])
  content = payload[13:]
  return addr, length, content


####### USB Reading
if "download" == args.command:
  data = download(0, 0x196b28)
  print(hexDump(data, "", 0))


elif "download_contacts" == args.command:
  contactData = download(0x65b70, 0xc000)

  table = PrettyTable(['Index','Name', 'Type', 'unk0', 'pad1', 'id', 'unk1', 'unk2'])

  for idx, name, type, unk0, pad1, id, unk1, unk2 in struct.iter_unpack('<H32sBBHIIH', contactData):
    typeName = "Unknown"
    if type == 0:
      typeName = 'Private Call'
    elif type == 1:
      typeName = 'Group Call'

    table.add_row([idx, name.decode('utf-8'), typeName, unk0, pad1, id, unk1, unk2])

  print(table)
