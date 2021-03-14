#!/usr/bin/env python3

import pyshark
import struct
import sys
import binascii

def hexDump(s):
  h = " ".join(map("{:02x}".format, s))
  t = ""
  for i in range(len(s)):
    c = s[i]
    if c>=0x20 and c<0x7f:
      t += chr(c)
    else:
      t += "."
  return( h + " | " + t)

def isWriteCommand(p):
  if not isDataPacket(p):
    return False
  data = getData(p)
  return 'W' == chr(data[0])

def isDataPacket(p):
  return ("host" == p.usb.src) and ("USB.CAPDATA" in p)

def getData(p):
  if not isDataPacket(p): 
    return None
  return binascii.a2b_hex(p["USB.CAPDATA_RAW"].value)


def dumpWriteCommand(p, nextaddr):
  if not isWriteCommand(p):
    return
  data = getData(p)
  res = struct.unpack(">cIB16sBB", data)
  addr = res[1]
  if nextaddr != addr:
    print((8+3+16*3+16+2)*"-")
  print("{:08X} : {}".format(addr, hexDump(res[3])))
  return addr+16

cap = pyshark.FileCapture(sys.argv[1], include_raw=True, use_json=True)
nextaddr = 0
for p in cap:
  if isWriteCommand(p):
    nextaddr = dumpWriteCommand(p, nextaddr)
