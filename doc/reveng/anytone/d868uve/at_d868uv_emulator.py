#!/usr/bin/env python3
#
# Emulate anytone d878uv radio to customer programming software. 
# Send intercepted data stream over network to server script for further investigation.
#
# This script connects to a virtual com port COM26 which is connected via a virtual
# null modem cable to the virtual com port COM18 which is used by the programming software.
# This virtual ports and cable can be provided by the COM0COM tool.
#
# Linux users can use
# socat -d -d pty,raw,echo=0,b4000000 pty,raw,echo=0,b4000000
# for emulating a virtual null modem cable.

import serial
import time
import sys
import struct 


# config
filebase = 'codeplug' 
filecount = 0
comport = 'COM6' # connected to COM18 with com0com. use COM18 in CPS


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

# parameters?
if len(sys.argv) == 2:
   comport = sys.argv[1]
elif len(sys.argv) >= 3:
   print("Usage: " + sys.argv[0] + ' [comport]')
   exit()



# open serial port
serialPort = None

try:
   print("Trying comport " + comport)
   serialPort = serial.Serial(port = comport, baudrate=4000000, bytesize=8, timeout=1, stopbits=serial.STOPBITS_ONE) # 115200 921600 4000000
except (err):
   print('ERR: Could not open port ' + comport)
   print("Usage: " + sys.argv[0] + ' servername [comport]: ' + err)
   exit()


out = None 
nextaddr = None 

# wait for data

try:
   
   while 1:

      command = ''

      command = serialPort.read()
      while serialPort.in_waiting > 0:
         command += serialPort.read()

      # respond to command on com port
      if ( len(command) == 0 ):
         pass

      elif ( command == b'PROGRAM'):
         print("Program session requested.")
         resp = b'QX\x06'
         serialPort.write(resp)
         filename = "{}_{:04}.hex".format(filebase, filecount)
         out = open(filename, "w")
         nextaddr = None 
         
      elif ( command == b'\x02' ):
         print("Device info requested.")
         resp = b'ID868UVE\x00V102\x00\x00\x06'
         serialPort.write(resp)

      elif ( command == b'R\x02\xfa\x00\x20\x10' ):
         print("Read special memory request.")
         resp = b'W\x02\xfa\x00\x20\x10\xff\xff\xff\xff\x00\x00\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x26\x06'
         serialPort.write(resp)
         
      elif ( command[0:4] == b'R\x02\xfa\x00' and command[5] == 16 ):
         # 0x02fa00..
         print("Read local information.")
         
         resp = b'W\x02\xfa\x00' + bytes([command[4]]) + b'\x10'
         
         if ( command[4] == 0x00 ):
            resp += b'\x00\x00\x00\x00\x01\x01\x01\x00\x00\x01\x01\x20\x20\x20\x20\xff'
                                           
         elif ( command[4] == 0x10 ): # Radio Type
            resp += b'\x44\x38\x37\x38\x55\x56\x00\x01\x00\xff\xff\xff\xff\xff\xff\xff'

         elif ( command[4] == 0x30 ): # Serial Number
            resp += b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
         elif ( command[4] == 0x40 ): # Production Date
            resp += b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
         elif ( command[4] == 0x50 ): # Manucfacture Code
            resp += b'\x31\x32\x33\x34\x35\x36\x37\x38\xff\xff\xff\xff\xff\xff\xff\xff'
         elif ( command[4] == 0x60 ): # Maintained Date
            resp += b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
         elif ( command[4] == 0x70 ): # Dealer Code
            resp += b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
         elif ( command[4] == 0x80 ): # Stock Date
            resp += b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
         elif ( command[4] == 0x90 ): # Sell Date
            resp += b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
         elif ( command[4] == 0xa0 ): # Seller
            resp += b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'

         elif ( command[4] == 0xb0 ): # Maintained Description
            resp += b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
         elif ( command[4] == 0xc0 ): 
            resp += b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
         elif ( command[4] == 0xd0 ): 
            resp += b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
         elif ( command[4] == 0xe0 ): 
            resp += b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
         elif ( command[4] == 0xf0 ): 
            resp += b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'

         else:
            resp += b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'

         resp = resp + bytes( [sum(resp[1:]) & 0xff] ) + b'\x06'
         #print(resp.hex())
         serialPort.write(resp)

      elif ( command[0] == ord('W') ) :
         resp = b'\x06' # just ack
         serialPort.write(resp)
         res = struct.unpack(">cIB16sBB", command)
         addr = res[1]
         if nextaddr != addr:
            out.write((8+3+16*3+16+2)*"-" + "\n")
         out.write("{:08X} : {}\n".format(addr, hexDump(res[3])))
         nextaddr = addr+16
         
      elif ( command == b'END' ):
         print("End session.")
         resp = b'\x06' # just ack
         serialPort.write(resp)
         out.close()
         filecount += 1

      elif ( command == b'UPDATE' ):
         # for firmware update the device has to be switched on while pressing PF3 (blue button on top) and PTT keys
         print("Start Firmware Update. Only useful if device is in update receiving mode. (Switch on while pressing PF3 (blue button on top) and PTT keys)")
         resp = b'\x06' # just ack
         serialPort.write(resp)

      elif ( command == b'\x18' ):
         print("Firmware Update Send Complete. Switch device on while pressing PF2 (top left side) and PTT keys to start installer.")
         resp = b'\x06' # just ack
         serialPort.write(resp)

      elif ( command[0] == 0x01 ):
         print("Firmware data.")
         resp = b'\x06' # just ack
         serialPort.write(resp)
      else:
         #print("> " + str(command))
         pass



finally:
   print('QRT')
   serialPort.close()

