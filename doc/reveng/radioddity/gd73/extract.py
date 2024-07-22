#!/usr/bin/env python3

import pyshark
import struct
import sys
import binascii

device = "1.255"
device_in = "{}.2".format(device)
device_out = "{}.1".format(device)

def hexDump(s: bytes, prefix:str="", addr:int=0, compact:bool=False) -> str:
  """ Utility function to hex-dump binary data. """
  N = len(s)
  Nb = N//16
  if (N%16): Nb += 1
  res = ""
  last_line = None
  compressing = False
  for j in range(Nb):
    a,b = j*16, min((j+1)*16,N)
    line = s[a:b]
    if (not compact) or (last_line != line):
      h = " ".join(map("{:02x}".format, line))
      h += "   "*(16-(b-a))
      t = ""
      for i in range(a,b):
        c = s[i]
        if c>=0x20 and c<0x7f:
          t += chr(c)
        else:
          t += "."
      res += (prefix + "{:08X} ".format(addr+16*j) + h + " | " + t + "\n")
      last_line = line
      compressing = False
    elif not compressing: 
      res += (prefix + "*\n")
      compressing = True 
  return res[:-1]

def isDataPacket(p):
  return ("USB.CAPDATA" in p)

def isFromHost(p):
  return (("host" == p.usb.src) and (device_in == p.usb.dst))

def isFromDevice(p):
  return (("host" == p.usb.dst) and (device_out == p.usb.src))

def getData(p):
  if not isDataPacket(p): 
    return None
  return binascii.a2b_hex(p["USB.CAPDATA_RAW"].value)


class Packet:
  def __init__(self, data:bytes):
    self._flag, self._cmd, self._sub, self._pcrc, size = struct.unpack("<xBBBHH", data[:8])
    self._payload = bytes()
    if size: self._payload = data[8:(8+size)]
    self._crc = 0xffff
    for i in range(len(data)//2):
      v = struct.unpack("<H", data[(2*i):(2*(i+1))])[0]
      if v > self._crc: self._crc += 0xffff;
      self._crc -= v    
    if len(data)%2:
      v = struct.unpack("B", data[-1:])[0]
      if v > self._crc: self._crc +=0xffff;
      self._crc -= v;
    

  def __repr__(self):
    if 0 == len(self._payload):
      return "Packet flag={:02x}, cmd={:02x}, sub={:02x}, crc={:04x}.".format(self._flag, self._cmd, self._sub, self._crc)
    return "Packet flag={:02x}, cmd={:02x}, sub={:02x}, crc={:04x}\n{}".format(self._flag, self._cmd, self._sub, self._crc, hexDump(self._payload, " "))

cap = pyshark.FileCapture(sys.argv[1], include_raw=True, use_json=True)
nextaddr = 0
for p in cap:
  if isDataPacket(p):
    print(Packet(getData(p)))
