#!/usr/bin/env python3

import pyshark
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
parser.add_argument("cap_file", help="Path to the captured data.")
parser.add_argument("--nodump", help="Hide data dumps.", action="count", default=0)
args = parser.parse_args()

def download(segments):
  READ_SZ = 0x400
  TIMEOUT = 1500
  OUT_EP = 0x04
  IN_EP = 0x84

  dev = usb.core.find(idVendor=0x238b, idProduct=0x0a11)

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

  def readSegment(start_address, sz):
    end_address = start_address + sz
    ret = bytes()

    for addr in range(start_address, end_address, READ_SZ):
      length = READ_SZ if (addr + READ_SZ) < end_address else end_address - addr
      dev.write(OUT_EP, compose_read_command(addr, length), TIMEOUT)
      data = dev.read(IN_EP, 0x700, TIMEOUT)

      cmd, flag, src, des, unknown, length, payload = unpackLayer0Data(data)
      plen = len(payload)
      if (0x04 != cmd) or (7 > plen):
        raise ValueError('Expected response packet')

      # unpack layer 1
      u1, f1, what, u2, crc, end, plen, payload = unpackRequestResponse(payload)
      if 0xC7 != what:
        raise ValueError('Expected only read data')

      # unpack layer 2
      addr, length, crc_comp, payload = unpackReadWriteResponse(payload)

      ret += payload

    return ret

  segmentData = [readSegment(x[0], x[1]) for x in segments]

  dev.write(OUT_EP, b'\x7e\x01\x00\x00\x20\x10\x00\x00\x00\x14\xf4\x0f\x02\xc6\x01\x01\x00\x00\x6a\x03')
  dev.read(IN_EP, 1024, TIMEOUT)

  return segmentData

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

def isDataPacket(p):
  return ("USB.CAPDATA" in p)

def isFromHost(p):
  return ("host" == p.usb.src)

def isToHost(p):
  return ("host" == p.usb.dst)

def getData(p):
  if not isDataPacket(p): 
    return None
  return binascii.a2b_hex(p["USB.CAPDATA_RAW"].value)

def unpackLayer0(p):
  if not isDataPacket(p):
    return None
  data = getData(p)
  return unpackLayer0Data(data)

def unpackLayer0Data(data):
  cmd, flag, src, des, unknown, length = struct.unpack(">xBxBBBHH", data[:10])
  payload = data[10:]
  return cmd, flag, src, des, unknown, length, payload

def unpackRequestResponse(payload):
  msb, u1, f1, what, u2, length = struct.unpack("<BBBBBH", payload[:7])
  lsb, end = struct.unpack("BB", payload[-2:])
  return u1, f1, what, u2, ((msb<<8)|lsb), end, length, payload[7:-2]

def unpackReadWriteRequest(payload):
  ukn1, ukn2, ukn3, ukn4, addr, length = struct.unpack("<bHHbIH", payload[:12])
  crc = 0x59fd - (addr&0xffff) - (addr>>16) - length - 0x01
  content = payload[12:]
  for i in range(len(content)):
    crc -= content[i]
  return ukn1, ukn2, ukn3, ukn4, addr, length, crc&0xffff, content

def unpackReadWriteResponse(payload):
  addr, length = struct.unpack("<xxxxxxxIH", payload[:13])
  crc = 0x59fd - (addr&0xffff) - (addr>>16) - length - 0x01
  content = payload[13:]
  for i in range(len(content)):
    crc -= content[i]
  return addr, length, crc&0xffff, content

def isReadRequest(p):
  return isDataPacket(p) and isFromHost(p) and (0x01 == getData(p)[1])

def isReadResponse(p):
  return isDataPacket(p) and isToHost(p) and (0x04 == getData(p)[1])


if "serial" == args.command:
  print("#")
  print("# Serial data from '{0}'".format(args.cap_file))
  print("# '>' means from host to device.")
  print("# '<' means from device to host.")
  print("#")
  cap = pyshark.FileCapture(args.cap_file, include_raw=True, use_json=True)
  for p in cap:
    if not isDataPacket(p):
      continue
    if isFromHost(p):
      print("-"*80)
    hexdat = hexDump(getData(p))
    if isFromHost(p):
      print(hexDump(getData(p), "> | "))
    elif isToHost(p):
      print(hexDump(getData(p), "< | "))
      
elif "payload" == args.command:
  print("#")
  print("#")
  cap = pyshark.FileCapture(args.cap_file, include_raw=True, use_json=True)
  for p in cap:
    if not isDataPacket(p):
      continue
    cmd, flag, src, des, rcount, tlen, payload = unpackLayer0(p)
    plen = len(payload)

    dir = None
    if isFromHost(p): dir = ">"
    elif isToHost(p): dir = "<"

    if 0x00 == cmd: cmd = "CMD"
    elif 0x01 == cmd: cmd = "REQ"
    elif 0x04 == cmd: cmd = "RES"

    if (">" == dir):
      print("-"*80) 
    
    print("{} type={}, len={:04X}, plen={:04X}, rcount={:04X}:".format(dir, cmd, tlen, plen, rcount))
    if (("REQ" == cmd) or ("RES"==cmd)) and plen>9:
      u1, f1, what, u2, crc, end, plen, payload = unpackRequestResponse(payload)
      print("  | crc={:04X}, ukn1={:02X}, fixed={:02X}, what={:02X}, ukn2={:02X}, payload len={:04X}".format(crc, u1, f1, what, u2, plen))

      if (0xC7 == what) and ("REQ"==cmd): # read request
        ukn1, ukn2, ukn3, ukn4, addr, length, crc_comp, payload = unpackReadWriteRequest(payload); crc_comp-=u1
        if crc!=crc_comp:
          crc = "\x1b[1;31m{:04X} ERR\x1b[0m".format(crc_comp)
        else: 
          crc = "{:04X}".format(crc_comp) 
        print("  | READ from={:08X}, len={:04X}, crc={}".format(addr, length, crc))
        if not args.nodump:
          print(hexDump(payload, "  |  | "))
      elif (0xC7 == what) and ("RES"==cmd): # read response
        addr, length, crc_comp, payload = unpackReadWriteResponse(payload); crc-=u1
        if crc!=crc_comp:
          crc = "{:04X} ERR".format(crc_comp)
        else: 
          crc = "{:04X}".format(crc_comp) 
        print("  | READ from={:08X}, len={:04X}, crc={}".format(addr, length, crc))
        if not args.nodump:
          print(hexDump(payload, "  |  | "))
      elif (0xC8 == what) and ("REQ"==cmd):
        ukn1, ukn2, ukn3, ukn4, addr, length, crc_comp, payload = unpackReadWriteRequest(payload)
        if crc!=crc_comp:
          crc = "{:04X} ERR".format(crc_comp)
        else: 
          crc = "{:04X}".format(crc_comp) 
        print("  | WRITE to={:08X}, len={:04X}, crc={}".format(addr, length, crc))
        if not args.nodump:
          print(hexDump(payload, "  |  | "))
      elif (0xC8 == what) and ("RES"==cmd):
        addr, length, crc_comp, payload = unpackReadWriteResponse(payload)
        if crc!=crc_comp:
          crc = "{:04X} ERR".format(crc_comp)
        else: 
          crc = "{:04X}".format(crc_comp) 
        print("  | WRITE to={:08X}, len={:04X}, crc={}".format(addr, length, crc))
        if not args.nodump:
          print(hexDump(payload, "  |  | "))
      elif not args.nodump:
        print(hexDump(payload, "  | "))


elif "read_codeplug"==args.command:
  print("#")
  cap = pyshark.FileCapture(args.cap_file, include_raw=True, use_json=True)
  current_addr = 0xffffffff
  leftover = b''
  for p in cap:
    if (not isDataPacket(p)) or isFromHost(p):
      continue
    
    # unpack layer 0
    cmd, flag, src, des, ukn, tlen, payload = unpackLayer0(p)
    plen = len(payload)
    if (0x04 != cmd) or (9 > plen): # only handle responses
      continue
    
    # unpack layer 1
    u1, f1, what, u2, crc, end, plen, payload = unpackRequestResponse(payload)
    if 0xC7 != what: # filter only read data 
      continue

    # unpack layer 2
    addr, length, crc_comp, payload = unpackReadWriteResponse(payload)
    # Prepend left over bytes to fill up to 16b
    if len(leftover):
      payload  = leftover+payload
      addr    -= len(leftover)
      length  += len(leftover)
      leftover = b''

    # ensure payload length is multiple of 16
    if len(payload)%16:
      leftover = payload[-(len(payload)%16):]
      payload  = payload[:-(len(payload)%16)]
      length  -= len(leftover)

    # print
    if (0xffffffff != current_addr) and (current_addr != addr):
      if len(leftover):
        print(hexDump(leftover, "", current_addr))
        leftover = b''
      print("-"*75)
    print(hexDump(payload, "", addr))
    # update current address
    current_addr = addr+length
  
  if len(leftover):
    print(hexDump(payload, "", current_addr))


elif "write_codeplug"==args.command:
  print("#")
  cap = pyshark.FileCapture(args.cap_file, include_raw=True, use_json=True)
  current_addr = 0xffffffff
  for p in cap:
    if (not isDataPacket(p)) or isToHost(p):
      continue
    
    # unpack layer 0
    cmd, flag, src, des, ukn, tlen, payload = unpackLayer0(p)
    plen = len(payload)
    if (0x01 != cmd) or (8 > plen): # only handle requests
      continue
    
    # unpack layer 1
    u1, f1, what, u2, crc, end, plen, payload = unpackRequestResponse(payload)
    if 0xC8 != what: # filter only write data 
      continue

    # unpack layer 2
    addr, length, payload = unpackReadWriteRequest(payload)

    # if not continuos -> print separator
    if (current_addr != addr):
      print("-"*75)
    
    print(hexDump(payload, "", addr))
    
    # update current address
    current_addr = addr+length

####### USB Reading
elif "download" == args.command:
  data = download([(0, 0x196b28)])
  print(hexDump(data[0], "", 0))


elif "download_contacts" == args.command:
  data = download([(0x65b5c, 2), # Table Size
                   (0x65b6e, 2), # First contact index
                   (0x65b70, 0xc000)])

  tableElements = struct.unpack('<H', data[0])[0]
  currentIndex = struct.unpack('<H', data[1])[0]
  contactStructure = '<H32sBBHIIH'
  tableSize = tableElements * struct.calcsize(contactStructure)

  table = PrettyTable(['Index','Name', 'Type', 'unk0', 'pad1', 'id', 'unk1', 'link'])
  contactSlots = []

  # Build contact table.
  for idx, name, type, unk0, pad1, id, unk1, link in struct.iter_unpack(contactStructure, data[2][:tableSize]):
    contactSlots.append([idx, name.decode('utf-8'), type, unk0, pad1, id, unk1, link])

  assert len(contactSlots) == tableElements

  # Now traverse
  visitedIndexes = []
  while True:
    currentSlot = next(filter(lambda s: s[0] == currentIndex, contactSlots))
    link = currentSlot[7]
    type = currentSlot[2]

    if currentIndex in visitedIndexes:
      break

    visitedIndexes.append(currentIndex)

    typeName = "Unknown"
    if type == 0:
      typeName = 'Private Call'
    elif type == 1:
      typeName = 'Group Call'

      currentSlot[2] = typeName

    if type != 0x11:
      table.add_row(currentSlot)

    currentIndex = link

  print(table)
