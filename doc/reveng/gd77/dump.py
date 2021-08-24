#!/usr/bin/env python3


from json import dump
from re import S
import pyshark
import struct
import sys
import binascii

device = None # "1.13.0"

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
    b = b and (device == p.usb.dst)
  return b

def isFromDevice(p):
  b = ("host" == p.usb.dst)
  if device:
    b = b and (device == p.usb.src)
  return b

def isRequest(p):
  return isFromHost(p) and ("SETUP DATA" == p.highest_layer) and (p.layers[-1].has_field("data_fragment"))

def isResponse(p):
  return isFromDevice(p) and ("USB.CAPDATA" in p) 

def getData(p):
  if isRequest(p):
    return binascii.a2b_hex(p.layers[-1].data_fragment_raw[0])
  elif isResponse(p): 
    return binascii.a2b_hex(p["USB.CAPDATA_RAW"].value)
  return None


class RawPayload:
  def __init__(self, payload):
    self._payload = payload

  def __len__(self):
    return len(self._payload)

  def dump(self, prefix="", addr=0):
    return hexDump(self._payload, prefix, addr)
  

class Package:
  def __init__(self, payload):
    self._type, self._length = struct.unpack("<HH", payload[:4])
    payload = payload[4:(4+self._length)]
    if (0x52 == payload[0]) and len(payload)>=4:
      self._payload = ReadOperation(payload)
    elif (0x57 == payload[0]) and len(payload)>=4:
      self._payload = WriteOperation(payload)
    elif 0x41 == payload[0]:
      self._payload = ACK(payload)
    else:
      self._payload = RawPayload(payload)

  def type(self):
    return self._type

  def payload(self):
    return self._payload

  def dump(self, prefix=""):
    s = ""
    if 1 == self._type:
      s += prefix + "Request, len=0x{0:02X}".format(self._length) + "\n"
    elif 3 == self._type:
      s += prefix + "Response, len=0x{0:02X}".format(self._length) + "\n"
    s += self._payload.dump(prefix+"   | ")
    return s


class ReadOperation:
  def __init__(self, payload):
    self._addr, self._length = struct.unpack(">HB", payload[1:4])
    self._payload = None
    if 4 < len(payload):
      self._payload = RawPayload(payload[4:])

  def dump(self, prefix=""):
    s = prefix + "Read addr=0x{0:04X}, len=0x{1:02X}".format(self._addr, self._length)
    if None != self._payload:
      s += "\n" + self._payload.dump(prefix + "   | ")
    return s 

class WriteOperation:
  def __init__(self, payload):
    self._addr, self._length = struct.unpack(">HB", payload[1:4])
    self._payload = None
    if 4 < len(payload):
      self._payload = RawPayload(payload[4:])

  def payload(self):
    return self._payload
  def addr(self):
    return self._addr

  def dump(self, prefix=""):
    s = prefix + "Write addr=0x{0:04X}, len=0x{1:02X}".format(self._addr, self._length)
    if None != self._payload:
      s += "\n" + self._payload.dump(prefix + "   | ")
    return s 


class ACK: 
  def __init__(self, payload):
    pass 

  def dump(self, prefix=""):
    return prefix+"ACK"

if 2 == len(sys.argv):
  cap = pyshark.FileCapture(sys.argv[1], include_raw=True, use_json=True)
  for p in cap:
    if isRequest(p) and len(getData(p))>=4:
      P = Package(getData(p))
      print(P.dump(" > "))
      print("")
    elif isResponse(p) and len(getData(p))>=4:
      P = Package(getData(p))
      print(P.dump(" < "))
      print("")
    elif isRequest(p): 
      print(hexDump(getData(p), " > "))
    elif isResponse(p):
      print(hexDump(getData(p), " < "))

elif (3 == len(sys.argv)) and ("write" == sys.argv[1]):
  print("Dump written memory from file {0}:".format(sys.argv[2]))
  cap = pyshark.FileCapture(sys.argv[2], include_raw=True, use_json=True)
  addr = 0
  for p in cap:
    if isRequest(p) and len(getData(p))>=4:
      P = Package(getData(p))
      if isinstance(P.payload(), WriteOperation):
        W = P.payload()
        if (addr != W.addr()):
          print("")
          print ("-"*80)
          print("")
          addr = W.addr()
        print(W.payload().dump(" ", addr))
        addr += len(W.payload())
      else:
        print("")
        print(P.dump(" > "))
        print("")

else:
  print("oops")