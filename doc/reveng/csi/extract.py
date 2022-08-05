#!/usr/bin/env python3


from json import dump
from re import S
import pyshark
import struct
import sys
import binascii

devices = {11,15}

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



def handlePacket(data):
  # Unpack header 
  flags, cmd, length = struct.unpack(">xBBH", data[:5])
  crc = struct.unpack(">Hx", data[(5+length):])
  # Dispatch by flags and command
  if 0x00 == flags: 
    if 0x03 == cmd:
      return RadioInfoRequest(data[5:(5+length)], crc)
    if 0x10 == cmd:
      return ReadRequest(data[5:(5+length)], crc)
    if 0x11 == cmd:
      return WriteRequest(data[5:(5+length)], crc)
    if 0x14 == cmd:
      return BeforeWriteRequest(data[5:(5+length)], crc)
    if 0x15 == cmd:
      return BeforeReadRequest(data[5:(5+length)], crc)
  if 0x80 == flags:
    if 0x03 == cmd:
      return RadioInfoResponse(data[5:(5+length)], crc)
    if 0x10 == cmd:
      return ReadResponse(data[5:(5+length)], crc)
    if 0x11 == cmd:
      return WriteResponse(data[5:(5+length)], crc)
    if 0x14 == cmd:
      return BeforeWriteResponse(data[5:(5+length)], crc)
    if 0x15 == cmd:
      return BeforeReadResponse(data[5:(5+length)], crc)


class Request:
  def __init__(self, payload, crc):
    self._crc = crc 
    self._raw = payload

  def __repr__(self):
    return hexDump(self._raw, "> ")  


class Response:
  def __init__(self, payload, crc):
    self._crc = crc 
    self._raw = payload

  def __repr__(self):
    return hexDump(self._raw, "< ")  


class RadioInfoRequest(Request):
  def __init__(sefl, payload, crc):
    super().__init__(payload, crc)
    # no payload 
  
  def __repr__(sefl):
    return "> RadioInfo"


class RadioInfoResponse(Response):
  def __init__(self, payload, crc):
    super().__init__(payload, crc)
    self._status = payload[0]
    self._info = payload[1:]

  def __repr__(self):
    res = "< RadioInfo, status={0}\n".format(self._status)
    res += hexDump(self._info, " | ")
    return res 


class ReadRequest(Request):
  def __init__(self, payload, crc):
    super().__init__(payload, crc)
    self._addr, self._length = struct.unpack(">HH", payload)

  def __repr__(self):
    return "> Read addr={0:04x}, size={1:04x}".format(self._addr, self._length)


class ReadResponse(Response):
  def __init__(self, payload, crc):
    super().__init__(payload, crc)
    self._status, self._addr, self._unknown1, self._unknown2, self._unknown3 = struct.unpack(">BHHBB", payload[:7])
    self._data = payload[7:]

  def __repr__(self):
    res = "< Read status={0}, addr={1:04x}, ukn1={2:04x}, ukn2={3:02x}, ukn3={4:02x}\n".format(
      self._status, self._addr, self._unknown1, self._unknown2, self._unknown3)
    res += hexDump(self._data, " | ")
    return res 


class WriteRequest(Request):
  def __init__(self, payload, crc):
    super().__init__(payload, crc)
    self._addr, self._unknown1, self._unknown2, self._unknown3 = struct.unpack(">HHBB", payload[:6])
    self._data = payload[6:]

  def __repr__(self):
    res = "> Write addr={0:04x}, ukn1={1:04x}, ukn2={2:02x}, ukn3={3:02x}\n".format(
      self._addr, self._unknown1, self._unknown2, self._unknown3)
    res += hexDump(self._data, " | ")
    return res 


class WriteResponse(Response):
  def __init__(self, payload, crc):
    super().__init__(payload, crc)
    self._status = struct.unpack(">B", payload)

  def __repr__(self):
    return "< Write status={0}".format(self._status)


class BeforeWriteRequest(Request):
  def __init__(self, payload, crc):
    super().__init__(payload, crc)
    # no payload

  def __repr__(self):
    return "> BeforeWrite"


class BeforeWriteResponse(Response):
  def __init__(self, payload, crc):
    super().__init__(payload, crc)
    self._status, self._unknown1 = struct.unpack(">BB", payload)

  def __repr__(self):
    return "< BeforeWrite status={0}, ukn1={1:02x}".format(self._status, self._unknown1)


class BeforeReadRequest(Request):
  def __init__(self, payload, crc):
    super().__init__(payload, crc)
    # no payload

  def __repr__(self):
    return "> BeforeRead"

class BeforeReadResponse(Response):
  def __init__(self, payload, crc):
    super().__init__(payload, crc)
    self._status, self._unknown1 = struct.unpack(">BB", payload)

  def __repr__(self):
    return "< BeforeRead status={0}, ukn1={1:02x}".format(self._status, self._unknown1)


if "__main__" == __name__:
  if 3 != len(sys.argv):
    print("USAGE extract.py CMD CAP_FILENAME")
    sys.exit(-1)
  
  cmd = sys.argv[1]
  cap = pyshark.FileCapture(sys.argv[2])
  if "decode" == cmd:
    for pkt in cap:
      if len(devices) and not (int(pkt.usb.device_address) in devices):
        continue
      if not "scsi" in pkt:
        continue
      if ("scsi_sbc_opcode" not in pkt.scsi.field_names) or (0xff != int(str(pkt.scsi.get_field_value("scsi_sbc_opcode")))):
        continue 
      if "data" in pkt.scsi.field_names:
        print(handlePacket(binascii.a2b_hex(pkt.scsi.data.raw_value)))
  elif "dump" == cmd:
    for pkt in cap:
      if len(devices) and not (int(pkt.usb.device_address) in devices):
        continue
      if not "scsi" in pkt:
        continue
      if ("scsi_sbc_opcode" not in pkt.scsi.field_names) or (0xff != int(str(pkt.scsi.get_field_value("scsi_sbc_opcode")))):
        continue 
      if "data" not in pkt.scsi.field_names:
        continue
      msg = handlePacket(binascii.a2b_hex(pkt.scsi.data.raw_value))
      if not isinstance(msg, (ReadResponse, WriteRequest)):
        continue
      print(hexDump(msg._data, "", msg._addr*0x800))

    

