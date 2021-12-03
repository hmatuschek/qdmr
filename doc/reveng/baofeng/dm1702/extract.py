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


class Protocol:
  INIT    = 0
  ACK     = 1
  PSEARCH = 2
  PASSSTA = 3
  SYSINFO = 4
  VIEW    = 5
  GET     = 6
  READ    = 7
  WRITE   = 8

  def __init__(self, showACK=True, dataOnly=False):
    self._state = self.INIT 
    self._buffer = bytearray()
    self._showACK = showACK and (not dataOnly)
    self._dataOnly = dataOnly
    self._lastAddr = 0
    self._blockBuffer = bytearray()

  def clearBuffer(self):
    self._buffer = bytearray()

  def handleRequest(self, data):
    self._buffer += data
    if (self.INIT == self._state):
      if (b"PSEARCH" == self._buffer):
        self.handleSearchRequest()
      elif (b"PASSSTA" == self._buffer):
        self.handlePassStaRequest()
      elif (b'SYSINFO' == self._buffer):
        self.handleSysinfoRequest()
      elif (b"\x06" == self._buffer):
        self.handleACKRequest()
      elif (self._buffer.startswith(b"G")):
        self.handleGetRequest()
      elif (self._buffer.startswith(b"V")):
        self.handleViewRequest()
      elif (self._buffer.startswith(b"R")):
        self.handleReadRequest()
      else:
        print("> Unhandled request:")
        print(hexDump(self._buffer, ">  "))
        self.clearBuffer()
    elif self.PSEARCH == self._state:
      self.handleSearchRequest()
    elif self.PASSSTA == self._state:
      self.handlePassStaRequest()
    elif self.SYSINFO == self._state:
      self.handleSysinfoRequest()
    elif self.ACK == self._state:
      self.handleACKRequest()
    elif self.GET == self._state:
      self.handleGetRequest()
    elif self.VIEW == self._state:
      self.handleViewRequest()
    elif self.READ == self._state:
      self.handleReadRequest()

  def handleResponse(self, data):
    self._buffer += data
    if (self.PSEARCH == self._state):
      self.handleSearchResponse()
    elif (self.PASSSTA == self._state):
      self.handlePassStaResponse()
    elif (self.SYSINFO == self._state):
      self.handleSysinfoResponse()
    elif (self.ACK == self._state):
      self.handleACKResponse()
    elif (self.GET == self._state):
      self.handleGetResponse()
    elif self.VIEW == self._state:
      self.handleViewResponse()
    elif self.READ == self._state:
      self.handleReadResponse()
    else:
      print("< Unhandled response:")
      print(hexDump(self._buffer, "<  "))
      self.clearBuffer()
      self._state = self.INIT

  def handleSearchRequest(self):
    self._state = self.PSEARCH
    if not self._dataOnly:
      print("> Search") 
    self.clearBuffer()
  def handleSearchResponse(self):
    if ((8 == len(self._buffer)) and (0x06 == self._buffer[0])):
      if not self._dataOnly:
        print("< OK:")
        print(hexDump(self._buffer[1:], ">  "))
    elif not self._dataOnly:
      print("< ERR: Invalid search response:")
      print(hexDump(self._buffer, "<  "))
    self.clearBuffer()
    self._state = self.INIT

  def handlePassStaRequest(self):
    self._state = self.PASSSTA
    if not self._dataOnly:
      print("> PASSSTA")  
    self.clearBuffer()
  def handlePassStaResponse(self):
    if ((3 == len(self._buffer)) and (0x50==self._buffer[0])) and (not self._dataOnly):
      print("< OK: {:02X} {:02X}".format(int(self._buffer[0]), int(self._buffer[1])))
    elif not self._dataOnly:
      print("< Error:")
      print(hexDump(self._buffer, "<  "))
    self.clearBuffer()
    self._state = self.INIT 

  def handleSysinfoRequest(self):
    self._state = self.SYSINFO
    if not self._dataOnly:
      print("> System Info")  
    self.clearBuffer()
  def handleSysinfoResponse(self):
    if (1 == len(self._buffer)) and (0x06 == self._buffer[0]) and (not self._dataOnly):
      print("< ACK")
    elif not self._dataOnly:
      print("< Error:")
      print(hexDump(self._buffer, "<  "))
    self._state = self.INIT
    self.clearBuffer()

  def handleACKRequest(self):
    if self._showACK:
      print("> ACK?")
    self._state = self.ACK
    self.clearBuffer()
  def handleACKResponse(self):
    if (1 == len(self._buffer)) and (0x06 == self._buffer[0]) and self._showACK:
      print("< ACK!")
    elif not self._dataOnly:
      print("< Error:")
      print(hexDump(self._buffer, "<  "))
    self._state = self.INIT
    self.clearBuffer()
  
  def handleGetRequest(self):
    self._state = self.GET
    if (6 > len(self._buffer)):
      return
    addr0,addr12,length = struct.unpack("<xBHH", self._buffer[:6])
    addr = (addr12 << 8) | addr0
    if not self._dataOnly:
      print("> Get addr={:06X}, len={:04X}.".format(addr, length))
    self.clearBuffer()
  def handleGetResponse(self):
    if (6 > len(self._buffer)):
      return
    code,addr0,addr12,length = struct.unpack("<cBHH", self._buffer[:6])
    addr = (addr12 << 8) | addr0
    if ((6+length)>len(self._buffer)):
      return 
    if not self._dataOnly:
      print(hexDump(self._buffer[6:], "<  ", addr=addr))
    self._state = self.INIT
    self.clearBuffer()

  def handleViewRequest(self):
    self._state = self.VIEW
    if (5 > len(self._buffer)):
      return
    addr01,length,addr2 = struct.unpack("<xHBB", self._buffer[:5])
    self._lastAddr = addr = (addr2<<16) | addr01
    if not self._dataOnly:
      print("> View addr={:06X}, len={:02X}.".format(addr, length))
    self.clearBuffer()
  def handleViewResponse(self):
    if (3 > len(self._buffer)):
      return
    code,addr2,length = struct.unpack("<cBB", self._buffer[:3])
    if ((3+length)>len(self._buffer)):
      return 
    if not self._dataOnly:
      print(hexDump(self._buffer[3:], "<  ", addr=self._lastAddr))
    self._state = self.INIT
    self._lastAddr = 0
    self.clearBuffer()

  def handleReadRequest(self):
    self._state = self.READ
    if (6 > len(self._buffer)):
      return
    addr0,addr12,length = struct.unpack("<xBHH", self._buffer[:6])
    addr = (addr12 << 8) | addr0
    if not self._dataOnly:
      print("> Read addr={:06X}, len={:04X}.".format(addr, length))
    self.clearBuffer()
  def handleReadResponse(self):
    if (6 > len(self._buffer)):
      return
    code,addr0,addr12,length = struct.unpack("<cBHH", self._buffer[:6])
    addr = (addr12 << 8) | addr0
    if ((6+length)>len(self._buffer)):
      return 
    self.handleDataRead(addr, self._buffer[6:])
    self._state = self.INIT
    self.clearBuffer()


  def handleDataRead(self, addr, data):
    # Separate none-contin. reads if only data is dumped
    if not self._dataOnly:
      print(hexDump(data, "<  ", addr))
    else:
      bufferAddr = self._lastAddr - len(self._blockBuffer)
      if (addr != self._lastAddr):
        print(hexDump(self._blockBuffer, "<  ", bufferAddr))
        self._blockBuffer.clear()
        self._lastAddr = addr
        print("-"*80)      
      self._blockBuffer += data
      self._lastAddr += len(data)
      bufferAddr = self._lastAddr - len(self._blockBuffer)
      n = (len(self._blockBuffer)//16)*16
      if n:
        print(hexDump(self._blockBuffer[:n], "<  ", bufferAddr))
        self._blockBuffer = self._blockBuffer[n:]
        

if 3 > len(sys.argv):
  print("Usage extract (raw|proto|data) PCAPNG_FILE")
  sys.exit(-1)

what = sys.argv[1]
filename = sys.argv[2]
cap = pyshark.FileCapture(filename, include_raw=True, use_json=True)

if "raw" == what:
  for p in cap:
    if isRequest(p):
      print(hexDump(getData(p), " > "))
    elif isResponse(p):
      print(hexDump(getData(p), " < "))
elif "proto" == what:
  proto = Protocol()
  for p in cap:
    if isRequest(p):
      proto.handleRequest(getData(p))
    elif isResponse(p):
      proto.handleResponse(getData(p))
elif "data" == what:
  proto = Protocol(dataOnly=True)
  for p in cap:
    if isRequest(p):
      proto.handleRequest(getData(p))
    elif isResponse(p):
      proto.handleResponse(getData(p))
else: 
  print("Unknown extraction '{}', must be one of 'raw', 'proto' or 'data'.".format(what))
  sys.exit(-1)
    
