from packethandler import PacketHandler
from streamhandler import StreamHandler
from binascii import a2b_hex


class CDCACMFilter(PacketHandler):
  
  USB_URB_INTERRUPT = 0x01
  USB_URB_BULK      = 0x03

  def __init__(self):
    super().__init__()
    pass 

  def attach(self, handler:StreamHandler):
    self._handler.append(handler)
    return self

  def handle(self, packet):
    if (int(packet.usb.transfer_type,base=16) != CDCACMFilter.USB_URB_BULK) or ("USB.CAPDATA" not in packet):
      return
    for handler in self._handler:
      if PacketHandler.isFromHost(packet):
        handler.handleFromHost(a2b_hex(packet["USB.CAPDATA_RAW"].value))
      else:
        handler.handleToHost(a2b_hex(packet["USB.CAPDATA_RAW"].value))
