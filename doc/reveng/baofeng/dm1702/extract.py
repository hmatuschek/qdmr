#!/usr/bin/env python3

import pyshark
import struct
import sys
import binascii

def hexDump(s: bytes, prefix="", addr=0) -> str:
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

def isWriteCommand(p):
  if not isDataPacket(p):
    return False
  data = getData(p)
  return 'W' == chr(data[0])

def isRequest(p):
  return isDataPacket(p) and ("host" == p.usb.src)

def isResponse(p):
  return isDataPacket(p) and ("host" == p.usb.dst)

def isDataPacket(p):
  return ("USB.CAPDATA" in p)

def getData(p):
  if not isDataPacket(p): 
    return None
  return binascii.a2b_hex(p["USB.CAPDATA_RAW"].value)

cap = pyshark.FileCapture(sys.argv[1], include_raw=True, use_json=True)
nextaddr = 0
for p in cap:
  if isRequest(p):
    print(hexDump(getData(p), " > "))
  elif isResponse(p):
    print(hexDump(getData(p), " < "))

    
