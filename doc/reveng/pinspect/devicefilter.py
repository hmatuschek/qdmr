from packethandler import PacketHandler

class DeviceFilter(PacketHandler):
  """ Filters by device address. """

  def __init__(self, device: int = None):
    """ :param device: Specifies the USB device address of radio. """
    super().__init__()
    self._source = device

  def handle(self, packet):
    if ((None == self._source) or (int(packet.usb.device_address) == self._source)):
      super().handle(packet)
