from devicefilter import DeviceFilter
from streamhandler import StreamHandler
from cdcacmfilter import CDCACMFilter
from rawstreamdump import RawStreamDump
from datagram import Datagram, RawDatagramDump, DatagramHandler
from struct import unpack
from utilities import hexDump
from binascii import crc_hqx

def crc16_simple_sum8(data:bytearray):
  return sum(data)&0xffff

def crc16_simple_sum16(data:bytearray):
  s = 0
  for i in range(len(data)//2):
    s += unpack(">H", data[2*i:(2*(i+1))])[0]
  return s&0xffff

def crc16_simple_xor16(data:bytearray):
  s = 0
  for i in range(len(data)//2):
    s ^= unpack(">H", data[2*i:(2*(i+1))])[0]
  return s

def crc_summary(data:bytearray):
  print("CRC16-HQX  : {:04X}".format(crc_hqx(data, 0)))
  print("CRC16-SUM8 : {:04X}".format(crc16_simple_sum8(data)))
  print("CRC16-SUM16: {:04X}".format(crc16_simple_sum16(data)))
  print("CRC16-XOR16: {:04X}".format(crc16_simple_xor16(data)))


class AuctusA6Datagram(Datagram):
  def __init__(self, command, parameters, crc):
    self._isRequest = (0 == (command & 0x8000))
    self._command = (command & 0x7fff)
    self._parameters = parameters 
    self._crc = crc 

  def isRequest(self):
    return self._isRequest

  def isResponse(self):
    return not self.isRequest()

  def command(self):
    return self._command 

  def parameters(self):
    return self._parameters

  def format(self, prefix=""):
    res = prefix
    res += "type={}, command={:04X}, CRC={:02X}".format(
      ("Req" if self._isRequest else "Resp"), self._command, self._crc)
    if len(self._parameters):
      res += "\n"+hexDump(self._parameters, prefix=" "*len(prefix)+" |")
    return res 

  def __repr__(self):
    return self.format()
    

class AuctusA6Handler(StreamHandler):
  MODE_PASS     = 0x00
  MODE_DATAGRAM = 0x01

  def __init__(self):
    super().__init__()
    self._tobuffer = bytearray()
    self._frombuffer = bytearray()
    self._mode = AuctusA6Handler.MODE_DATAGRAM
    self._dataLeft = None
    self._handler = []

  def attach(self, handler):
    self._handler.append(handler)
    return self 

  def handleToHost(self, data:bytearray):
    self._tobuffer.extend(data)
    if AuctusA6Handler().MODE_DATAGRAM == self._mode: 
      while AuctusA6Handler().hasDatagram(self._tobuffer):
        dgram = AuctusA6Handler().popDatagram(self._tobuffer)
        self._processToHost(dgram)
        for handler in self._handler:
          if isinstance(handler, DatagramHandler):
            handler.handleToHost(dgram)
    else:
      self._dataLeft -= len(data)
      if 0 == self._dataLeft:
        self._mode = AuctusA6Handler.MODE_DATAGRAM
        for handler in self._handler:
          if isinstance(handler, StreamHandler):
            handler.handleToHost(self._tobuffer)
        crc_summary(self._tobuffer)
        self._tobuffer.clear()

  def handleFromHost(self, data:bytearray):
    self._frombuffer.extend(data)
    if AuctusA6Handler().MODE_DATAGRAM == self._mode: 
      while AuctusA6Handler().hasDatagram(self._frombuffer):
        dgram = AuctusA6Handler().popDatagram(self._frombuffer)
        self._processFromHost(dgram)
        for handler in self._handler:
          if isinstance(handler, DatagramHandler):
            handler.handleFromHost(dgram)
    else:
      self._dataLeft -= len(data)
      if 0 == self._dataLeft:
        self._mode = AuctusA6Handler.MODE_DATAGRAM
        for handler in self._handler:
          if isinstance(handler, StreamHandler):
            handler.handleFromHost(self._frombuffer)
        crc_summary(self._frombuffer)
        self._frombuffer.clear()

  @staticmethod 
  def hasDatagram(buffer: bytearray):
    if not buffer.startswith(b'\xaa'): return False 
    if len(buffer) < 6: return False
    length, command = unpack(">xBH", buffer[:4])
    return len(buffer) >= length

  @staticmethod
  def popDatagram(buffer: bytearray):
    length, command = unpack(">xBH", buffer[:4])
    length, command, params, crc = unpack(">xBH{}sBx".format(length-6), buffer[:length])
    del buffer[:length]
    return AuctusA6Datagram(command, params, crc)

  def _processFromHost(self, dgram:AuctusA6Datagram):
    if dgram.isRequest() and (0x0102 == dgram.command()):
      self._dataLeft, baudRate = unpack(">xxIxxI", dgram.parameters())
    elif dgram.isRequest() and (0x0101 == dgram.command()):
      self._mode = AuctusA6Handler.MODE_PASS

  def _processToHost(self, dgram:AuctusA6Datagram):
    if dgram.isResponse() and (0x0102 == dgram.command()):
      self._mode = AuctusA6Handler.MODE_PASS
    elif dgram.isResponse() and (0x0100 == dgram.command()):
      self._dataLeft, = unpack(">xI10x", dgram.parameters())

if "__main__" == __name__:
  import sys
  stream = DeviceFilter(14).attach(
        CDCACMFilter().attach(
          AuctusA6Handler()
            .attach(RawDatagramDump())
            .attach(RawStreamDump())))
  stream.process(sys.argv[1])
