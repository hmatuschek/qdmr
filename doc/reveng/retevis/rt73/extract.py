#!/usr/bin/env python3

#!/usr/bin/env python3


from json import dump
from re import S
import pyshark
import struct
import sys
import binascii

device = "5.3."

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

def isFromHost(p):
  b = ("host" == p.usb.src)
  if device:
    b = b and p.usb.dst.startswith(device)
  return b

def isFromDevice(p):
  b = ("host" == p.usb.dst)
  if device:
    b = b and p.usb.src.startswith(device)
  return b

def isRequest(p):
  return isFromHost(p) and ("USB.CAPDATA" in p) 

def isResponse(p):
  return isFromDevice(p) and ("USB.CAPDATA" in p) 

def getData(p):
  return binascii.a2b_hex(p["USB.CAPDATA_RAW"].value)



cap = pyshark.FileCapture(sys.argv[1], include_raw=True, use_json=True)
raddr = 0
waddr = 0
for p in cap:
  if isRequest(p):
    d = getData(p)
    print(hexDump(d, "> ", waddr))
    waddr += len(d)
  elif isResponse(p):
    d = getData(p)
    print(hexDump(d, "< ", raddr))
    raddr += len(d)
