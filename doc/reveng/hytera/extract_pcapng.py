#!/usr/bin/env python3 

import pyshark  # type: ignore
import binascii
from packet import *


import argparse
parser = argparse.ArgumentParser()
parser.add_argument("command", help="""What to extract. Possible commands are: serial (dumps serial data in hex).
""")
parser.add_argument("cap_file", help="Path to the captured data.")
parser.add_argument("--nodump", help="Hide data dumps.", action="count", default=0)
args = parser.parse_args()

def isDataPacket(p):
  return ("USB.CAPDATA" in p)

def isFromHost(p):
  return ("host" == p.usb.src)

def isToHost(p):
  return ("host" == p.usb.dst)

def getData(p):
  if not isDataPacket(p): 
    return None
  return binascii.a2b_hex(p["USB.CAPDATA_RAW"].value)


if "raw" == args.command:
  print("#")
  print("# Serial data from '{0}'".format(args.cap_file))
  print("# '>' means from host to device.")
  print("# '<' means from device to host.")
  print("#")
  cap = pyshark.FileCapture(args.cap_file, include_raw=True, use_json=True)
  for p in cap:
    if not isDataPacket(p):
      continue
    if isFromHost(p):
      print("-"*80)
    print(hexDump(getData(p)))
      
elif "structure" == args.command:
  print("#")
  print("#")
  cap = pyshark.FileCapture(args.cap_file, include_raw=True, use_json=True)
  for p in cap:
    if not isDataPacket(p):
      continue
    packet = Packet.unpack(getData(p))
    if isFromHost(p):
      print("-"*80)
    print(packet)

elif "fw_structure" == args.command:
  print("#")
  print("#")
  HOST = 0
  DEV = 1
  cap = pyshark.FileCapture(args.cap_file, include_raw=True, use_json=True)

  # It seems as though single FW packets are split over multiple USB packets in
  # firmware mode. Concatenate them, assuming that we can use host packets as
  # delimeters.
  currentData = bytes()

  # Assume that first data source is host
  lastDataSource = HOST

  for p in cap:
    currentDataSource = DEV

    if not isDataPacket(p):
      continue

    if isFromHost(p):
      currentDataSource = HOST

    if currentDataSource != lastDataSource:
      fwPacket = FirmwarePacket.unpack(currentData)
      print(fwPacket)
      currentData = bytes()

      lastDataSource = currentDataSource
      if currentDataSource == HOST:
        print('-'*80)

    currentData += getData(p)
