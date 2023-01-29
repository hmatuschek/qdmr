#!/usr/bin/env python3

import pyshark
import sys
import binascii
from abc import ABCMeta

dev_addr = 1

if len(sys.argv) != 2:
    print("Usage: exract.py PCAPNG_FILE")


def hexDump(s: bytes, prefix: str = "", addr: int = 0) -> str:
  """ Utility function to hex-dump binary data. """
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
    if isFromHost(p):
      print(hexDump(getData(p), "< "))
    else:
      print(hexDump(getData(p), "> "))
      print(p)
        
