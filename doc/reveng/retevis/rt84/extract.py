#!/usr/bin/env python3 

import struct 
import sys 


def hexDump(s: bytes, prefix="", addr=0, compact=False) -> str:
  """ Utility function to hex-dump binary data. """
  N = len(s)
  Nb = N//16
  if (N%16): Nb += 1
  res = ""
  last_line = None
  compressing = False
  for j in range(Nb):
    a,b = j*16, min((j+1)*16,N)
    line = s[a:b]
    if (not compact) or (last_line != line):
      h = " ".join(map("{:02x}".format, line))
      h += "   "*(16-(b-a))
      t = ""
      for i in range(a,b):
        c = s[i]
        if c>=0x20 and c<0x7f:
          t += chr(c)
        else:
          t += "."
      res += (prefix + "{:08X} ".format(addr+16*j) + h + " | " + t + "\n")
      last_line = line
      compressing = False
    elif not compressing: 
      res += (prefix + "*\n")
      compressing = True 
  return res[:-1]


if 2 > len(sys.argv):
  print("USAGE: extract.py CODEPLUG.rdt")
rdt_filename = sys.argv[1]

# list of sections: (file offset, size, destination address)
sections = [
  (0x002225, 0x03e000, 0x002000),
  (0x040230, 0x090000, 0x110000),
]

# Read all
data = open(rdt_filename, "br").read()
for section in sections:
  offset, length, addr = section
  print(hexDump(data[offset:(offset+length)], "", addr, True))


