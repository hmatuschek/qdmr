from abc import ABCMeta, abstractmethod


class Datagram(metaclass=ABCMeta):
  """ Base class of all data grams. """

  def __init__(self):
    pass 

  @abstractmethod
  def format(self, prefix=""):
    pass


class DatagramHandler(metaclass=ABCMeta):

  def __init__(self):
    pass 

  @abstractmethod
  def handleFromHost(self, dgram: Datagram):
    pass 

  @abstractmethod 
  def handleToHost(self, dgram: Datagram):
    pass 


class RawDatagramDump(DatagramHandler):
  def __init__(self):
    super().__init__()

  def handleFromHost(self, dgram:Datagram):
    print(dgram.format("< "))

  def handleToHost(self, dgram:Datagram):
    print(dgram.format("> "))
