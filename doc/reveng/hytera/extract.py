#!/usr/bin/env python3

import pyshark
import struct
import sys
import binascii

import argparse
parser = argparse.ArgumentParser()
parser.add_argument("command", help="""What to extract. Possible commands are: serial (dumps serial data in hex).
""")
parser.add_argument("cap_file", help="Path to the captured data.")

args = parser.parse_args()


def hexDump(s):
  h = " ".join(map("{:02x}".format, s))
  t = ""
  for i in range(len(s)):
    c = s[i]
    if c>=0x20 and c<0x7f:
      t += chr(c)
    else:
      t += "."
  return( h + " | " + t)

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

def unpackPacket(p):
  if not isDataPacket(p):
    return None
  data = getData(p)
  cmd, flag, src, des, unknown, length = struct.unpack(">xBxBBBHH", data[:10])
  payload = data[10:(length-2)]
  sum, ack = struct.unpack("BB", data[-2:])
  return cmd, flag, src, des, unknown, length, payload, sum, ack

def isReadRequest(p):
  return isDataPacket(p) and isFromHost(p) and (0x01 == getData(p)[1])

def isReadResponse(p):
  return isDataPacket(p) and isToHost(p) and (0x04 == getData(p)[1])


if "serial" == args.command:
  print("#")
  print("# Serial data from '{0}'".format(args.cap_file))
  print("# '>' means from host to device.")
  print("# '<' means from device to host.")
  print("#")
  cap = pyshark.FileCapture(args.cap_file, include_raw=True, use_json=True)
  for p in cap:
    if not isDataPacket(p):
      continue
    hexdat = hexDump(getData(p))
    if isFromHost(p):
      print("> | {0}".format(hexdat))
    elif isToHost(p):
      print("< | {0}".format(hexdat))
      
elif "payload" == args.command:
  print("#")
  print("#")
  cap = pyshark.FileCapture(args.cap_file, include_raw=True, use_json=True)
  for p in cap:
    if not isDataPacket(p):
      continue;
    cmd, flag, src, des, ukn, tlen, payload, sum, ack = unpackPacket(p)
    plen = len(payload)

    dir = None;
    if isFromHost(p): dir = ">"
    elif isToHost(p): dir = "<"

    if 0x00 == cmd: cmd = "CMD"
    elif 0x01 == cmd: cmd = "RD"
    elif 0x04 == cmd: cmd = "RES"

    print("-"*80)
    print("{0} cmd={1}, len={2:04X}, plen={3:04X}, crc={4:02X}, ack={5:02X}, ukn={6:04X}:".format(dir, cmd, tlen, plen, sum, ack, ukn))
    print("{0} | {1}".format(dir, hexDump(payload)))
