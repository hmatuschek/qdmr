from utilities import hexDump
from streamhandler import StreamHandler


class RawStreamDump(StreamHandler):
  def __init__(self):
    super().__init__()
    self._toAddr = 0
    self._fromAddr = 0

  def handleFromHost(self, data):
    print(hexDump(data, "< ", self._fromAddr))
    self._fromAddr+=len(data)

  def handleToHost(self, data):
    print(hexDump(data, "> ", self._toAddr))
    self._toAddr+=len(data)
