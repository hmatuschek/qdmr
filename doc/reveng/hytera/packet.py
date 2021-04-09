""" Module implementing the assembly and disassembly/parsing of Hytera packets.

The most important class here is Packet. This class provides static methods to
assemble and parse *any* packets used in the communication with the Hytera devices.

>>> from packet import Packet
>>> p = Packet.connect()
>>> print(Packet.unpack(p.pack()))
> CMD: flags=FE, src=20, dest=10, seq=0000, crc=E560 (OK)
>>> # send p.pack(), receive response into data
>>> r = Packet.unpack(data)
>>> # inspect packet if you want to
>>> print(r)
< RES: flags=FD, src=10, dest=20, seq=0000, crc=D270 (OK)
"""

import struct

def hexDump(s, prefix="", addr=0):
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



class Packet:
  """Represents a packet send to or received from the device.

  Use the static methods connect(), readCodeplug(), writeCodeplug() and reboot() to assemble
  request packets or unpack to parse received packets. """
  CONNECT = 0x00
  REQ     = 0x01
  RES     = 0x04

  DEVICE = 0x10
  HOST   = 0x20

  FLAGS_DEFAULT     = 0x00
  FLAGS_CONNNECT    = 0xfe
  FLAGS_RES_CONNECT = 0xfd

  def __init__(self, ptype, flags, src, dest, seq, content, crc=0):
    """ Contstructs a new packet.

    To construct a packet to send to the device, consider using one of the static helper methods.

    Args:
      ptype (int): Specifies the packet type. One of CMD, REQ or RES.
      flags (int): Specifies additional flags (only used for ptype=CMD). One of FLAGS_DEFAULT, FLAGS_CONNECT or FLAGS_RES_CONNECT.
      src (int): Specifies the packet source (usually HOST). One of HOST or DEVICE.
      dest (int): Specifies the packet destination (usually DEVICE). One of HOST or DEVICE.
      seq (int): Response sequence number. This is set to 0 for all requests. Every response will increase this counter by one.
      content (Object): The packet content. An instance of either EmptyContent, UnknownData or Request
      crc (int): Optional CRC value for the packet. Will be recomputed when calling pack().
    """
    self._type    = ptype
    self._flags   = flags
    self._src     = src
    self._dest    = dest
    self._seq     = seq
    self._crc     = crc
    self._content = content

  def pack(self):
    """ Packs the packet into a bytes object. """
    payload = self._content.pack()
    tlength = 12 + len(payload)
    crc = 0
    header = struct.pack(">BBBBBBHH", 0x7e, self._type, 0x00, self._flags, self._src, self._dest, self._seq, tlength)
    header += struct.pack("<H", crc)
    crc = self.crc(header + payload)
    # repack header with correct crc
    header = struct.pack(">BBBBBBHH", 0x7e, self._type, 0x00, self._flags, self._src, self._dest, self._seq, tlength)
    header += struct.pack("<H", crc)
    return header + payload

  @staticmethod
  def unpack(data):
    """ Unpacks the given bytes string. """
    f1, ptype, f2, flags, src, dest, seq, tlength = struct.unpack(">BBBBBBHH", data[:10])
    (crc,) = struct.unpack("<H", data[10:12])
    payload = data[12:]
    assert (0x7e == f1)
    assert (0x00 == f2)
    assert (tlength==len(data))
    if (Packet.CONNECT == ptype) or ((Packet.RES == ptype) and (0x00 != flags)):
      return Packet(ptype, flags, src, dest, seq, EmptyContent(), crc)
    elif (Packet.REQ==ptype) or ((Packet.RES == ptype) and (0x00 == flags)):
      return Packet(ptype, flags, src, dest, seq, Request.unpack(payload), crc)

  @staticmethod
  def crc(packet):
    """ Computes the CRC over the given packed packet. """
    s = 0
    for i in range(5):
      s += struct.unpack("<H", packet[(2*i):(2*(i+1))])[0]
    for i in range(6,len(packet)//2):
      s += struct.unpack("<H", packet[(2*i):(2*(i+1))])[0]
    if len(packet)%2:
      s += packet[-1]
    while s > 0xffff:
      s = (s>>16) + (s&0xffff)
    return s^0xffff

  def __str__(self):
    """ Returns a string representation of the packet. """
    return self.dump()

  def __len__(self):
    """ Returns the length of the packed packet. """
    return 12+len(self._content)

  def dump(self, prefix=""):
    """ Returns a string representation of the packet.

    Each line is prefixed with the given prefix."""
    s = prefix
    if (Packet.CONNECT == self._type):
      s += "> CONNECT: "
    elif (Packet.REQ == self._type):
      s += "> REQ: "
    elif (Packet.RES == self._type):
      s += "< RES: "
    crc_fmt = "{:04X}".format(self._crc)
    crc_comp = self.crc(self.pack())
    if self._crc == crc_comp:
      crc_fmt += " (OK)"
    else:
      crc_fmt += " (ERR: \x1b[1;31m{:02X}\x1b[0m)".format(crc_comp)
    s += "flags={:02X}, src={:02X}, dest={:02X}, seq={:04X}, crc={}\n".format(self._flags, self._src, self._dest, self._seq, crc_fmt)
    return s + self._content.dump(prefix + "     | ")

  @staticmethod
  def connect():
    """ Assembles a command request to the radio to connect. """
    return Packet(Packet.CONNECT, Packet.FLAGS_CONNNECT, Packet.HOST, Packet.DEVICE, 0, EmptyContent())

  @staticmethod
  def getRadioID():
    """ Assembles a request for the radio to identify itself. """
    return Packet(Packet.REQ, Packet.FLAGS_DEFAULT, Packet.HOST, Packet.DEVICE, 0,
      Request(Request.GET_RADIO_ID,
        GetStringRequest(0x00)))

  @staticmethod
  def getVersion():
    """ Assembles a version string request. """
    return Packet(Packet.REQ, Packet.FLAGS_DEFAULT, Packet.HOST, Packet.DEVICE, 0,
      Request(Request.GET_VERSION,
        GetStringRequest(0x12)))

  @staticmethod
  def enterProgMode(mode):
    """Instruct the radio to enter programming mode.

    This packet needs to be sent in order for any read or writes to codeplug
    memory to occur. The response contains the 'head' information which
    includes, model number, serial number and firmware version amongst other
    data.

    The request type is 0x01c5 and the payload is just 17 x 0x00.
    """
    return Packet(Packet.REQ, Packet.FLAGS_DEFAULT, Packet.HOST, Packet.DEVICE, 0,
      Request(Request.ENTER_PROG_MODE, EnterProgModeRequest(mode)))

  @staticmethod
  def readCodeplug(addr, length):
    """ Assembles a read codeplug memory request for the given memory address and length to read. """
    return Packet(Packet.REQ, Packet.FLAGS_DEFAULT, Packet.HOST, Packet.DEVICE, 0,
      Request(Request.READ_CODEPLUG,
        ReadCodeplugRequest(addr, length)))

  @staticmethod
  def writeCodeplug(addr, payload):
    """ Assembles a write request to the codeplug memory at the specified address and writing the given payload. """
    return Packet(Packet.REQ, Packet.FLAGS_DEFAULT, Packet.HOST, Packet.DEVICE, 0,
      Request(Request.WRITE_CODEPLUG,
        WriteCodeplugRequest(addr, payload)))

  @staticmethod
  def leaveProgMode():
    """ Assembles a request to the radio to leave programming mode.

    NOTE: this causes the radio to be rebooted when in WRITE mode."""
    return Packet(Packet.REQ, Packet.FLAGS_DEFAULT, Packet.HOST, Packet.DEVICE, 0,
      Request(Request.LEAVE_PROG_MODE,
        LeaveProgModeRequest()))


class FirmwarePacket:
  """Represents a firmware packet send to or received from the device."""

  def __init__(self, content, crc=0):
    """ Contstructs a new packet.

    To construct a packet to send to the device, consider using one of the static helper methods.

    Args:
      ptype (int): Specifies the packet type, a 16-bit LE value who's function is unknown
      content (Object): The packet content. An instance of Request.
      crc (int): Optional CRC value for the packet. Will be recomputed when calling pack().
    """
    self._crc     = crc
    self._content = content

  def pack(self):
    """ Packs the packet into a bytes object. """
    payload = self._content.pack()
    tlength = 4 + len(payload)
    crc = 0
    header = struct.pack("<HH", tlength, crc)
    crc = self.crc(header + payload)
    # repack header with correct crc
    header = struct.pack("<HH", tlength, crc)
    return header + payload

  @staticmethod
  def unpack(data):
    """ Unpacks the given bytes string. """
    tlength, crc = struct.unpack("<HH", data[:4])
    assert tlength == len(data)
    payload = data[4:]
    return FirmwarePacket(Request.unpack(payload, True), crc)

  @staticmethod
  def crc(packet):
    """ Computes the CRC over the given packed packet. """
    s = (~len(packet)) & 0xffff
    for i in range(2,len(packet)//2):
      s += (~(struct.unpack("<H", packet[(2*i):(2*(i+1))])[0])) & 0xffff
    if len(packet)%2:
      s += (~(packet[-1] << 8)) & 0xffff
    return s&0xffff

  def __str__(self):
    """ Returns a string representation of the packet. """
    return self.dump()

  def __len__(self):
    """ Returns the length of the packed packet. """
    return 4+len(self._content)

  def dump(self, prefix=""):
    """ Returns a string representation of the packet.

    Each line is prefixed with the given prefix."""
    s = prefix
    s += "FW: "
    crc_fmt = "{:04X}".format(self._crc)
    crc_comp = self.crc(self.pack())
    if self._crc == crc_comp:
      crc_fmt += " (OK)"
    else:
      crc_fmt += " (ERR: \x1b[1;31m{:02X}\x1b[0m diff:{:05X})".format(crc_comp, self._crc - crc_comp)
    s += "crc={}\n".format(crc_fmt)
    return s + self._content.dump(prefix + "     | ")

  @staticmethod
  def readCodeplug(addr, length):
    """ Assembles a read codeplug memory request for the given memory address and length to read. """
    return FirmwarePacket(Request(Request.FW_READ_MEMORY,
                                  FwReadMemoryRequest(0x03, addr, length)))

  @staticmethod
  def writeCodePlug(addr, payload):
    """ Assembles a write codeplug memory request for the given memory address and length. """
    return FirmwarePacket(Request(Request.FW_WRITE_MEMORY,
                                  FwWriteMemoryRequest(0x03, addr, payload)))

  @staticmethod
  def setMemoryAccess(enableAccess):
    return FirmwarePacket(Request(Request.FW_SET_MEM_ACCESS,
                                  FwMemoryAccessRequest(enableAccess)))

class EmptyContent:
  """ Tiny helper class to represent some empty packet/request content. """
  def __init__(self):
    pass

  def pack(self):
    return b""

  @staticmethod
  def unpack(data):
    assert(0 == len(data))
    return EmptyContent()

  def __str__(self):
    return ""

  def __len__(self):
    return 0

  def dump(self, prefix=""):
    return ""



class UnknownData:
  """ Helper class representing some raw data. This can be used to represent some unknown raw packet content. """
  def __init__(self, data):
    """ Constructor.

    Args:
      data (bytes): The raw data.
    """
    self._data = data

  def pack(self):
    return self._data

  @staticmethod
  def unpack(data):
    return UnknownData(data)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return len(self._data)

  def dump(self, prefix=""):
    s = prefix + "Raw data len={:04X}\n".format(len(self))
    s += hexDump(self._data, prefix)
    return s



class Request:
  REQUEST_RESPONSE = 0x8000
  FW_READ_MEMORY = 0x01c2
  FW_WRITE_MEMORY= 0x01c3
  ENTER_PROG_MODE     = 0x01c5
  LEAVE_PROG_MODE           = 0x01c6
  READ_CODEPLUG    = 0x01c7
  WRITE_CODEPLUG   = 0x01c8
  GET_VERSION      = 0x0201
  GET_RADIO_ID     = 0x0203
  FW_SET_MEM_ACCESS= 0x0218

  def __init__(self, rtype, content, crc=None):
    self._rtype = rtype
    self._content = content

    if crc == None:
      self._crc = 0
    else:
      self._crc = crc


  def pack(self):
    crc = 0
    payload = self._content.pack()
    header = struct.pack("<BHH", 0x02, self._rtype, len(payload))
    footer = struct.pack("BB", crc, 0x03)
    crc = self.crc(header + payload + footer)
    footer = struct.pack("BB", crc, 0x03)
    return header + payload + footer

  @staticmethod
  def unpack(data, isFirmware=False):
    f1, rtype, plen = struct.unpack("<BHH", data[:5])
    crc, f2 = struct.unpack("BB", data[-2:])
    payload = data[5:-2]
    assert (0x02 == f1) and (0x03==f2) and (plen==(len(data)-7))
    # assert (crc == self.computeCRC())

    # Firmware request unpacking
    if isFirmware:
      if Request.FW_READ_MEMORY == rtype:
        return Request(rtype, FwReadMemoryRequest.unpack(payload), crc)
      elif (Request.FW_READ_MEMORY | Request.REQUEST_RESPONSE) == rtype:
        return Request(rtype, FwReadMemoryResponse.unpack(payload), crc)
      elif Request.FW_SET_MEM_ACCESS == rtype:
        return Request(rtype, FwMemoryAccessRequest.unpack(payload), crc)
      elif (Request.FW_SET_MEM_ACCESS | Request.REQUEST_RESPONSE) == rtype:
        return Request(rtype, FwMemoryAccessResponse.unpack(payload), crc)
      elif Request.FW_WRITE_MEMORY == rtype:
        return Request(rtype, FwWriteMemoryRequest.unpack(payload), crc)
      elif (Request.FW_WRITE_MEMORY | Request.REQUEST_RESPONSE) == rtype:
        return Request(rtype, FwWriteMemoryResponse.unpack(payload), crc)
      else:
        return Request(rtype,UnknownData(payload), crc)

    if Request.READ_CODEPLUG == rtype:
      return Request(rtype, ReadCodeplugRequest.unpack(payload), crc)
    elif (Request.READ_CODEPLUG | Request.REQUEST_RESPONSE) == rtype:
      return Request(rtype, ReadCodeplugResponse.unpack(payload), crc)
    elif Request.WRITE_CODEPLUG == rtype:
      return Request(rtype, WriteCodeplugRequest.unpack(payload), crc)
    elif (Request.WRITE_CODEPLUG | Request.REQUEST_RESPONSE) == rtype:
      return Request(rtype, WriteCodeplugResponse.unpack(payload), crc)
    elif (Request.GET_RADIO_ID == rtype) or (Request.GET_VERSION == rtype):
      return Request(rtype, GetStringRequest.unpack(payload), crc)
    elif ((Request.GET_RADIO_ID | Request.REQUEST_RESPONSE) == rtype) or ((Request.GET_VERSION | Request.REQUEST_RESPONSE) == rtype):
      return Request(rtype, GetStringResponse.unpack(payload), crc)
    elif Request.LEAVE_PROG_MODE == rtype:
      return Request(rtype, LeaveProgModeRequest.unpack(payload), crc)
    elif (Request.LEAVE_PROG_MODE | Request.REQUEST_RESPONSE) == rtype:
      return Request(rtype, LeaveProgModeResponse.unpack(payload), crc)
    elif Request.ENTER_PROG_MODE == rtype:
      return Request(rtype, EnterProgModeRequest.unpack(payload), crc)
    elif (Request.ENTER_PROG_MODE | Request.REQUEST_RESPONSE) == rtype:
      return Request(rtype, EnterProgModeResponse.unpack(payload), crc)
    else:
      return Request(rtype, UnknownData(payload), crc)

  @staticmethod
  def crc(packet):
    s = 0
    for i in range(1,len(packet)-2):
      s += packet[i]
    s = ((~s)+0x33)&0xff
    return s

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 7 + len(self._content)

  def dump(self, prefix=""):
    s  = prefix
    if Request.REQUEST_RESPONSE & self._rtype:
      s += "RSP: "
    else:
      s += "REQ: "
    crc_note = "OK"
    crc_comp = self.crc(self.pack())
    if self._crc != crc_comp:
      crc_note = "\x1b[1;31m{:02X}\x1b[0m".format(crc_comp)
    s += "type={:04X} crc={:02X} ({})\n".format(
      self._rtype, self._crc, crc_note)
    return s + self._content.dump(prefix + "   | ")



class ReadCodeplugRequest:
  def __init__(self, addr, length):
    self._addr = addr
    self._length = length

  def pack(self):
    return struct.pack("<BBBBBBIH", 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, self._addr, self._length)

  @staticmethod
  def unpack(data):
    f1,f2,f3,f4,f5,f6, addr, length = struct.unpack("<BBBBBBIH", data[:12])
    assert ( (0x00 == f1) and (0x00 == f2) and (0x00 == f3) and
             (0x01 == f4) and (0x00 == f5) and (0x00 == f6) )
    return ReadCodeplugRequest(addr, length)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 12

  def dump(self, prefix=""):
    s  = prefix
    s += "RD: addr={:08X} len={:04X}\n".format(self._addr, self._length)
    return s


class FwReadMemoryRequest:
  def __init__(self, memory, addr, length):
    self._memory = memory
    self._addr = addr
    self._length = length

  def pack(self):
    return struct.pack('<BIH', self._memory, self._addr, self._length)

  @staticmethod
  def unpack(data):
    memory, addr, length = struct.unpack('<BIH',  data)
    return FwReadMemoryRequest(memory, addr, length)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 7

  def dump(self, prefix=""):
    s  = prefix
    s += "FWRD: memory={:02X} addr={:08X} len={:04X}\n".format(self._memory, self._addr, self._length)
    return s

class ReadCodeplugResponse:
  def __init__(self, addr, payload=None):
    self._addr = addr
    self._payload = payload

  def pack(self):
    header = struct.pack("<BBBBBBBIH", 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, self._addr, len(self._payload))
    return header + self._payload

  @staticmethod
  def unpack(data):
    f1,f2,f3,f4,f5,f6,f7, addr, length = struct.unpack("<BBBBBBBIH", data[:13])
    payload = data[13:]
    assert ( (0x00 == f1) and (0x00 == f2) and (0x00 == f3) and
             (0x00 == f4) and (0x01 == f5) and (0x00 == f6) and
             (0x00 == f7) and (length==len(payload)) )
    return ReadCodeplugResponse(addr, payload)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 13+len(self._payload)

  def dump(self, prefix=""):
    s  = prefix
    s += "RD: addr={:08X}\n".format(self._addr)
    return s + hexDump(self._payload, prefix+"  | ", self._addr)

class FwReadMemoryResponse:
  STATUS_SUCCESS = 0x00

  def __init__(self, status, memory, addr, payload=None):
    self._status = status
    self._memory = memory
    self._addr = addr
    self._payload = payload

  def pack(self):
    header = struct.pack("<BBIH", self._status, self._memory, self._addr, len(self._payload))
    return header + self._payload

  @staticmethod
  def unpack(data):
    status, memory, addr, length = struct.unpack("<BBIH", data[:8])
    payload = data[8:]

    if status == FwReadMemoryResponse.STATUS_SUCCESS:
      assert length==len(payload)

    return FwReadMemoryResponse(status, memory, addr, payload)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 8+len(self._payload)

  def __getStatusMessage__(self):
    if self._status == FwReadMemoryResponse.STATUS_SUCCESS:
      return "SUCCESS"
    else:
      return "{:02X}".format(self._status)

  def dump(self, prefix=""):
    s  = prefix
    s += "FWRD: status={} memory={:02X} addr={:08X}\n".format(self.__getStatusMessage__(),
                                                              self._memory, self._addr)
    return s + hexDump(self._payload, prefix+"  | ", self._addr)


class FwWriteMemoryRequest:

  def __init__(self, memory, addr, payload=None):
    self._memory = memory
    self._addr = addr
    self._payload = payload

  def pack(self):
    header = struct.pack("<BIH", self._memory, self._addr, len(self._payload))
    return header + self._payload

  @staticmethod
  def unpack(data):
    memory, addr, length = struct.unpack("<BIH", data[:7])
    payload = data[7:]

    assert length==len(payload)

    return FwWriteMemoryRequest(memory, addr, payload)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 7+len(self._payload)

  def dump(self, prefix=""):
    s  = prefix
    s += "FWWR: memory={:02X} addr={:08X}\n".format(self._memory, self._addr)
    return s + hexDump(self._payload, prefix+"  | ", self._addr)

class FwWriteMemoryResponse:
  STATUS_SUCCESS = 0x00
  MEM_CP = 0x03

  def __init__(self, status, memory, addr, length):
    self._status = status
    self._memory = memory
    self._addr = addr
    self._length = length

  def pack(self):
    return struct.pack("<BBIH", self._status, 0x00, self._addr, self._length)

  @staticmethod
  def unpack(data):
    status, memory, addr, length = struct.unpack("<BBIH", data)

    return FwWriteMemoryResponse(status, memory, addr, length)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 7+len(self._payload)

  def __getStatusMessage__(self):
    if self._status == FwReadMemoryResponse.STATUS_SUCCESS:
      return "SUCCESS"
    else:
      return "{:02X}".format(self._status)

  def dump(self, prefix=""):
    s  = prefix
    s += "FWWR: status={} memory={:02X} addr={:08X} len={:04X}\n".format(self.__getStatusMessage__(),
                                                                         self._memory, self._addr, self._length)
    return s

class FwMemoryAccessRequest:
  DISABLE_ACCESS = 0x0000
  ENABLE_ACCESS = 0x10fb

  """Construct an enable access request.

  enableAccess = True to enable access, False otherwise."""
  def __init__(self, enableAccess):
    self._enableAccess = enableAccess

  def pack(self):
    return struct.pack("<H", FwMemoryAccessRequest.ENABLE_ACCESS if self._enableAccess \
                             else FwMemoryAccessRequest.DISABLE_ACCESS)

  @staticmethod
  def unpack(data):
    x = struct.unpack("<H", data)[0]
    enableAccess = False

    assert (x == FwMemoryAccessRequest.DISABLE_ACCESS or
            x == FwMemoryAccessRequest.ENABLE_ACCESS)

    if x == FwMemoryAccessRequest.ENABLE_ACCESS:
      enableAccess = True

    return FwMemoryAccessRequest(enableAccess)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 2

  def __getStatusMessage__(self):
    if self._status == FwReadMemoryResponse.STATUS_SUCCESS:
      return "SUCCESS"
    else:
      return "{:02X}".format(self._status)

  def dump(self, prefix=""):
    s  = prefix
    stateString = "Enable" if self._enableAccess else "Disable"
    s += "MemoryAccessRequest: {} access\n".format(stateString)
    return s

class FwMemoryAccessResponse:
  STATUS_OK = 0x5e

  def __init__(self, status):
    self._status = status

  def pack(self):
    return struct.pack("<B", self._status)

  @staticmethod
  def unpack(data):
    status = struct.unpack("B", data)[0]
    return FwMemoryAccessResponse(status)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 1

  def dump(self, prefix=""):
    s  = prefix
    statusText = 'SUCCESS'
    if not self._status == FwMemoryAccessResponse.STATUS_OK:
      statusText = "ERR ({:02X})".format(self._status)
    s += "MemoryAccessRequestResponse: status={}\n".format(statusText)
    return s

class WriteCodeplugRequest:
  def __init__(self, addr, payload):
    self._addr = addr
    self._payload = payload

  def pack(self):
    length = len(self._payload)
    header = struct.pack("<BBBBBBIH", 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, self._addr, length)
    return header + self._payload

  @staticmethod
  def unpack(data):
    f1,f2,f3,f4,f5,f6, addr, length = struct.unpack("<BBBBBBIH", data[:12])
    assert ( (0x00 == f1) and (0x00 == f2) and (0x00 == f3) and
             (0x01 == f4) and (0x00 == f5) and (0x00 == f6) )
    payload = data[12:]
    return WriteCodeplugRequest(addr, payload)

  def __str__(self):
    return self.dump()

  def __len__(self):
   return 12+len(self._payload)

  def dump(self, prefix=""):
    s  = prefix
    s += "WR: addr={:08X}\n".format(self._addr)
    return s + hexDump(self._payload, prefix+"  | ", self._addr)



class WriteCodeplugResponse:
  def __init__(self, addr, length):
    self._addr = addr
    self._length = length

  def pack(self):
    return struct.pack("<BBBBBBBIH", 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, self._addr, self._length)

  @staticmethod
  def unpack(data):
    f1,f2,f3,f4,f5,f6,f7, addr, length = struct.unpack("<BBBBBBBIH", data[:13])
    assert ( (0x00 == f1) and (0x00 == f2) and (0x00 == f3) and
             (0x00 == f4) and (0x01 == f5) and (0x00 == f6) and
             (0x00 == f7) )
    return WriteCodeplugResponse(addr, length)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 13

  def dump(self, prefix=""):
    s  = prefix
    s += "WR: addr={:08X} len={:04X}\n".format(self._addr, self._length)
    return s


class GetStringRequest:
  RADIO_ID = 0x00
  UNKNOWN_1 = 0x12
  UNKNOWN_2 = 0x09

  def __init__(self, what):
    self._what = what

  def pack(self):
    return struct.pack("B", self._what)

  @staticmethod
  def unpack(data):
    assert(1 == len(data))
    what, = struct.unpack("B",data)
    return GetStringRequest(what)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 1

  def dump(self, prefix=""):
    return prefix + "STR: what={:02X}".format(self._what)


class GetStringResponse:
  def __init__(self, ukn1, what, s):
    self._ukn1 = ukn1
    self._what = what
    self._string = str(s)

  def pack(self):
    header = struct.pack("BBBBBBBB",self._ukn1,0,0,0,self._what,0,0,0)
    return header + self._string.encode("utf_16_le")

  @staticmethod
  def unpack(data):
    ukn1,f2,f3,f4,what,f6,f7,f8 = struct.unpack("BBBBBBBB",data[:8])
    assert (0==f2) and (0==f3) and (0==f4) and (0==f6) and (0==f7) and (0==f8)
    return GetStringResponse(ukn1, what,data[8:].decode("utf_16_le"))

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 8+32

  def dump(self, prefix=""):
    res  = prefix + "STR: what={:02X}, ukn1={:02X}, len={:04X}\n".format(self._what, self._ukn1, len(self._string))
    res += prefix + "   | " + self._string.replace("\u0000","")
    return res


class LeaveProgModeRequest:
  def __init__(self, ukn1=0x00):
    self._ukn1 = ukn1

  def pack(self):
    return struct.pack("B", self._ukn1)

  @staticmethod
  def unpack(data):
    assert(1 == len(data))
    ukn1, = struct.unpack("B", data)
    return LeaveProgModeRequest(ukn1)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 1

  def dump(self, prefix=""):
    return prefix + "LEAVE_PROG_MODE: ukn1={:02X}".format(self._ukn1)


class LeaveProgModeResponse:
  def __init__(self, ukn1=0x00):
    self._ukn1 = ukn1

  def pack(self):
    return struct.pack("B", self._ukn1)

  @staticmethod
  def unpack(data):
    assert(1 == len(data))
    ukn1, = struct.unpack("B", data)
    return LeaveProgModeRequest(ukn1)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 1

  def dump(self, prefix=""):
    return prefix + "LEAVE_PROG_MODE: ukn1={:02X}".format(self._ukn1)


class EnterProgModeRequest:
  READ_MODE = 0x00
  WRITE_MODE = 0x02

  def __init__(self, mode):
    self._mode = mode

  def pack(self):
    return struct.pack("BBBBBBBBBBBBBBBBB", self._mode,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)

  @staticmethod
  def unpack(data):
    mode,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16,f17 = struct.unpack("BBBBBBBBBBBBBBBBB", data)
    assert ((mode == EnterProgModeRequest.READ_MODE) or
            (mode == EnterProgModeRequest.WRITE_MODE))
    return EnterProgModeRequest(mode)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 17

  def __geModeString__(self):
    if self._mode == EnterProgModeRequest.READ_MODE:
      return 'READ'
    elif self._mode == EnterProgModeRequest.WRITE_MODE:
      return 'WRITE'
    else:
      return 'UNKOWN ({:02X})'.format(self._mode)

  def dump(self, prefix=""):
    return prefix + "ENTER_PROG_MODE: mode={}".format(self.__geModeString__())


class EnterProgModeResponse:
  READ_MODE = 0x00
  WRITE_MODE = 0x02

  def __init__(self, status, mode, headData):
    self._status = status
    self._mode = mode
    self._headData = headData

  def pack(self):
    payload = self._headData
    header = struct.pack("<BBH", 0, self._mode, len(payload))
    return header+payload

  @staticmethod
  def unpack(data):
    status, mode, length = struct.unpack("<BBH", data[:4])
    payload = data[4:]
    assert ((mode == EnterProgModeResponse.READ_MODE) or
            (mode == EnterProgModeResponse.WRITE_MODE))

    if status == 0x00:
      assert length == len(payload)

    return EnterProgModeResponse(status, mode, payload)

  def __str__(self):
    return self.dump()

  def __len__(self):
    return 4+len(self._headData)

  def __getStatusMessage__(self):
    if self._status == FwReadMemoryResponse.STATUS_SUCCESS:
      return "SUCCESS"
    else:
      return "{:02X}".format(self._status)

  def __getModeString__(self):
    if self._mode == EnterProgModeRequest.READ_MODE:
      return 'READ'
    elif self._mode == EnterProgModeRequest.WRITE_MODE:
      return 'WRITE'
    else:
      return 'UNKOWN ({:02X})'.format(self._mode)

  def dump(self, prefix=""):
    res  = prefix + "ENTER_PROG_MODE: status={} mode={} headData:\n".format(self.__getStatusMessage__(),
                                                                            self.__getModeString__())
    return res + hexDump(self._headData, prefix+"  | ", 0)

if "__main__" == __name__:
  # Example script

  # "enter program mode" request and disassembly
  print(hexDump(Packet.enterProgramMode().pack()))
  print(Packet.unpack(Packet.enterProgramMode().pack()))
  print()
  # ID radio request an disassembly
  print(hexDump(Packet.getRadioID().pack()))
  print(Packet.unpack(Packet.getRadioID().pack()))
  print()
  # Read some unknown radio info
  print(hexDump(Packet.readUnknownRadioInfo().pack()))
  print(Packet.unpack(Packet.readUnknownRadioInfo().pack()))
  print()
  # Read some data request and disassembly
  print(hexDump(Packet.readCodeplug(0x00000000, 0x100).pack()))
  print(Packet.unpack(Packet.readCodeplug(0x00000000, 0x100).pack()))
  print()
  # reboot radio request and disassembly
  print(hexDump(Packet.reboot().pack()))
  print(Packet.unpack(Packet.reboot().pack()))
  print()
