#!/usr/bin/env python3

""" 
THis module implements some basic packet handler, that can filter and process packets captured by
wireshark. 
"""

from abc import ABCMeta, abstractmethod
from pyshark import FileCapture


class PacketHandler(metaclass=ABCMeta):
  """ Abstract base class for all packet handlers. """

  def __init__(self):
    self._handler = []

  def attach(self, handler):
    self._handler.append(handler)
    return self

  @staticmethod
  def isFromHost(p):
    return "host" == p.usb.src

  @staticmethod
  def isToHost(p):
    return "host" == p.usb.dest

  def handle(self, packet):
    for handler in self._handler:
      handler.handle(packet)

  def process(self, filename:str):
    for packet in FileCapture(filename, include_raw=True, use_json=True):
      self.handle(packet)
    