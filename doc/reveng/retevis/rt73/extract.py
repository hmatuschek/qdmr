#!/usr/bin/env python3

#!/usr/bin/env python3


from json import dump
from re import S
import pyshark
import struct
import sys
import binascii

device = "5.3."
#device = None

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


class Device:
  STATE_INIT = 0
  STATE_READ_START = 1
  STATE_READ = 2
  STATE_WRITE_START = 3
  STATE_WRITE = 4
  STATE_CHECKSUM = 5

  def __init__(self, maxSize=0x1c000):
    self._state = self.STATE_INIT
    self._addr = 0
    self._rx = b''
    self._tx = b''  
    self._maxSize = maxSize 

  def handleRequest(self, data):
    if (self.STATE_INIT == self._state):
      if (data.startswith(b"Flash Read")):
        self._state = self.STATE_READ_START
      elif (data.startswith(b"Flash Write")):
        self._state = self.STATE_WRITE_START
      else:
        print("Unhandled request:")
        print(hexDump(data, " > "))
    elif (self.STATE_READ_START == self._state):
      if (data == b'Read'):
        print("Device info: ")
        print(hexDump(self._rx, " < ", 0))
        self._rx = b''
        print("Codeplug read:")
        self._state = self.STATE_READ
      else:
        print("Unhandled request:")
        print(hexDump(data, " > "))
    elif (self.STATE_READ == self._state):
      if (data == b'Read'):
        self._state = self.STATE_READ
      else:
        print("Unhandled request:")
        print(hexDump(data, " > "))
    elif (self.STATE_WRITE == self._state):
      self._tx += data
      self.dumpTX()

  
  def handleResponse(self, data):
    if (self.STATE_READ_START == self._state):
      self._rx += data
    elif (self.STATE_READ == self._state):
      self._rx += data
      self.dumpRX()
      if (self._rx.startswith(b"ChecksumR")):
        crc = struct.unpack("<I", self._rx[9:14])
        print("Checksum: {:08X}".format())
        self._rx = self._rx[14:]
        self._state = self.STATE_CHECKSUM
    elif (self.STATE_WRITE_START == self._state):
      self._rx += data
      if (0x5d <= len(self._rx)):
        print("Device info: ")
        print(hexDump(self._rx[:0x5d], " < "))
        print("Codeplug write: ")
        self._tx = b""
        self._rx = b""
        self._addr = 0
        self._state = self.STATE_WRITE
    elif (self.STATE_WRITE == self._state):
      self._rx += data
      if (self._rx.startswith(b"Write")):
        self._rx = self._tx[5:]
      elif (self._rx.startswith(b"ChecksumW")):
        crc = struct.unpack("<I", self._rx[9:14])
        print("Checksum: {:08X}".format())
        self._rx = self._rx[14:]
        self._state = self.STATE_CHECKSUM
    else:
      print("Unhandled response:")
      print(hexDump(data, " < "))
  
  def dumpRX(self):
    while 16 <= len(self._rx):
      print(hexDump(self._rx[:16], " < ", self._addr))
      self._rx = self._rx[16:]
      self._addr += 16

  def dumpTX(self):
    while 16 <= len(self._tx):
      print(hexDump(self._tx[:16], " > ", self._addr))
      self._tx = self._tx[16:]
      self._addr += 16
    

cap = pyshark.FileCapture(sys.argv[1], include_raw=True, use_json=True)
dev = Device()
rawOutput = ((3 == len(sys.argv)) and ("raw"==sys.argv[2]))
for p in cap:
  if isRequest(p):
    d = getData(p)
    if rawOutput:
      print(hexDump(d, "> "))
      print("-"*80)
    else:
      dev.handleRequest(d)
  elif isResponse(p):
    d = getData(p)
    if rawOutput:
      print(hexDump(d, "< "))
      print("-"*80)
    else: 
      dev.handleResponse(d)
