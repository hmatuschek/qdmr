#!/usr/bin/env python3

import pyshark
import struct
import sys
import binascii

dev_addr = 1

if len(sys.argv) != 2:
    print("Usage: exract.py PCAPNG_FILE")

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

def isFromHost(p):
    return ("host" == p.usb.src) and ((None==dev_addr) or (dev_addr == int(p.usb.device_address))) 

def isToHost(p):
    return (("host" == p.usb.dest) and ((None==dev_addr) or (dev_addr == int(p.usb.device_address))))

def isDataPacket(p):
  return ("USB.CAPDATA" in p)

def getData(p):
  if not isDataPacket(p): 
    return None
  return binascii.a2b_hex(p["USB.CAPDATA_RAW"].value)


cap = pyshark.FileCapture(sys.argv[1], include_raw=True, use_json=True)
nextaddr = 0
for p in cap:
  if isDataPacket(p):
    dump = hexDump(getData(p))
    if isFromHost(p):
      print("< " + dump)
    else:
      print("> " + dump)
        
