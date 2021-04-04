import struct 

def hexDump(s, prefix="", addr=0):
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
  CMD = 0x00
  REQ = 0x01
  RES = 0x04

  DEVICE = 0x10
  HOST   = 0x20

  def __init__(self, ptype, flags, src, dest, seq, content, crc=0):
    self._type    = ptype
    self._flags   = flags
    self._src     = src
    self._dest    = dest 
    self._seq     = seq
    self._crc     = crc
    self._content = content

  def pack(self):
    payload = self._content.pack()
    tlength = 12 + len(payload)
    crc = 0
    header = struct.pack(">BBBBBBHH", 0x7e, self._type, 0x00, self._flags, self._src, self._dest, tlength, self._seq)
    header += struct.pack("<H", crc)
    crc = self.crc(header + payload)
    # repack header with correct crc
    header = struct.pack(">BBBBBBHH", 0x7e, self._type, 0x00, self._flags, self._src, self._dest, tlength, self._seq)
    header += struct.pack("<H", crc)
    return header + payload

  @staticmethod
  def unpack(data):
    f1, ptype, f2, flags, src, dest, seq, tlength = struct.unpack(">BBBBBBHH", data[:10])
    (crc,) = struct.unpack("<H", data[10:12])
    payload = data[12:]
    assert (0x7e == f1) 
    assert (0x00 == f2) 
    assert (tlength==len(data))
    if (Packet.CMD == ptype) or ((Packet.RES==ptype) and (0x00 != flags)):
      return Packet(ptype, flags, src, dest, seq, UnknownData.unpack(payload), crc)
    elif (Packet.REQ==ptype) or ((Packet.RES == ptype) and (0x00 == flags)):
      return Packet(ptype, flags, src, dest, seq, Request.unpack(payload), crc)

  @staticmethod
  def crc(packet):
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
    return self.dump()    

  def __len__(self):
    return 12+len(self._content)

  def dump(self, prefix=""):
    s = prefix
    if (Packet.CMD == self._type): 
      s += "> CMD: "
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

    
class UnknownData:
  def __init__(self, data):
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
    s = prefix + "Raw data len={}\n".format(len(self))
    s += hexDump(self._data, prefix)
    return s
    

class Request:
  REQUEST_RESPONSE = 0x8000
  READ_CODEPLUG    = 0x01c7
  WRITE_CODEPLUG   = 0x01c8
  GET_VERSION      = 0x0201
  GET_RADIO_ID     = 0x0203

  def __init__(self, rtype, content, crc=None):
    self._crc = crc
    self._rtype = rtype
    self._content = content 

  def pack(self):
    crc = 0
    payload = self._content.pack()
    header = struct.pack("<BHH", 0x02, self._rtype, len(payload))
    footer = struct.pack("BB", crc, 0x03)
    crc = self.crc(header + payload + footer)
    footer = struct.pack("BB", crc, 0x03)
    return header + payload + footer

  @staticmethod
  def unpack(data):
    f1, rtype, plen = struct.unpack("<BHH", data[:5])
    crc, f2 = struct.unpack("BB", data[-2:])
    payload = data[5:-2]
    assert (0x02 == f1) and (0x03==f2) and (plen==(len(data)-7))
    # assert (crc == self.computeCRC())
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
    return GetStringResponse(ukn1, what,data[8:].decode("utf_16_le").replace("\u0000",""))
  
  def __str__(self):
    return self.dump()

  def __len__(self):
    return 8+32

  def dump(self, prefix=""):
    return prefix + "STR: what={:02X}, ukn1={:02X}\n".format(self._what, self._ukn1) + prefix + "   | " + self._string 
