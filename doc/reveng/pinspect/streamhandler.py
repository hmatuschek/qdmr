from abc import ABCMeta, abstractmethod

class StreamHandler(metaclass=ABCMeta):
  def __init__(self):
    pass 
  
  @abstractmethod
  def handleFromHost(self, data:bytearray):
    pass 

  @abstractmethod
  def handleToHost(self, data:bytearray):
    pass 

