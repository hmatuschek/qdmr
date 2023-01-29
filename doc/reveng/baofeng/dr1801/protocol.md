# DR-1801A6 (BF1801) - Protocol

A new version of the popular DM-1801 by Baofeng. Is sold as DR-1801UV or DR-1801A6 and calls itself
BF1801. The communication is based on USB CDC-ACM (serial over USB). 

This device uses the AUCTUS A6 chip and thus its protocol. For a detailed description, see jhart99s 
brilliant [article series](https://jhart99.com/a6-hidden-interface/) on this chip.

## Basic structure
The first few packets exchanged at a read:
```
> aa 06 01 04 03 bb 
< aa 07 81 04 01 83 bb

> aa 07 00 2b 00 2c bb 
< aa 07 80 2b 02 ae bb 

> aa 0a 01 00 00 01 c2 00 c8 bb 
< aa 15 81 00 01 00 01 dd 90 00 00 00 68 00 02 e6 9e 09 2d ef bb 

> aa 06 01 01 06 bb 
< ... only packets from device to host from now on with no structure. Just plain data.
```

The first few packets exchanged at a write:
```
> aa 06 01 04 03 bb
< aa 07 81 04 01 83 bb 

> aa 07 00 2b 00 2c bb 
< aa 07 80 2b 02 ae bb

> aa 12 01 02 00 01 00 01 da 8c 7f f6 00 01 c2 00 0d bb
< aa 08 81 02 01 00 8a bb 

> .... Codeplug is written in many large packets with no structure, just data.

< aa 09 81 03 01 00 00 8a bb

< 00 | .
< 00 | .
< 00 | .
```

There appears to be two kind of messages command/response messages and status messages from the 
device. The status messages do not appear to be triggered by by the host. 

## Command/response messages
They start with `aah` and end with `bbh`. The general structure of command/response messages are
```
 +---+---+---+---+---+...+---+---+---+
 |aah|LEN|  CMD  | Params    |CRC|bbh|
 +---+---+---+---+---+...+---+---+---+
```
The `LEN` field contains the length of the entire packet, including start and stop bytes. The 
parameter field is optional and of variable length. The checksum is computed by simply xor-ing the 
entire payload byte-wise. The command field has the following sub-structure:
```
 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 |C/R| Command 15bit, big-endian                                 |
 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
```
The most significant bit contains a flag, that is set for responses and cleared for requests.

There are many commands already known see jhart99s 
[list of commands](https://jhart99.com/atecps/#cps-commands).


### 0000h Command -- Identify radio
Not used by the manufacturer CPS but handled by the radio. This command is send without any 
parameters and the radio returns a sting, identifying the radio. The response looks like
```
aa 3a 80 00 01 20 2c 42   46 31 38 30 31 2c 41 36 	..... ,B  F1801,A6
2d 30 30 30 30 2d 58 58   58 58 2c 70 6f 72 74 61 	-0000-XX  XX,porta
62 6c 65 2c 31 33 36 4d   2d 31 37 34 4d 2c 34 30 	ble,136M  -174M,40
30 4d 2d 34 38 30 4d 2c   fd bb                     0M-480M,  ..
```

The response payload starts with a status byte (0x01 success) followed by an ASCII string, 
containing the information about the radio. The information is separated by comma (,). 
The first entry is empty (single space), followed by the device name (BF1801), followed by
the firmware version (weird number here), device class (portable) and the frequency ranges. 
There is likely space for three bands, however, the device only supports 136-174MHz and 400-480MHz.


### 0104h Command -- Enter CPS mode (?)
 First command send without any parameters irrespective of codeplug read or write.  
 
 Response contains a single parameter `01h`, likely status byte like *success*. 
 

### 002bh Command -- Check Programming Password
Checks the programming password. Request contains parameter `00h`. Likely, the length of the 
password. Needs to be tested with a proper password. 

The Response contains a single byte `02h`. 

### 0100h Command 
Only send before reading the codeplug. Contains 4bytes of parameters. Seen 
`00 01 c2 00` = 115200 big endian. Set UART speed?!? 

Response parameters are quiet long, e.g. 
`01 00 01 dd 90 00 00 00 68 00 02 e6 9e 09 2d`. 0x0001dd90 is the size of the codeplug file. Then,
it appears like two uint32 numbers i.e., 0x00000068 and 0x0002e69e. These numbers do not change. The
last two bytes, however, appear to be a checksum.

### 0101h Command
Only send right before reading of the codeplug starts. There is no actual response to this request,
the radio responds with the codeplug. No parameters send with this command.

### 0102h Command  
Only send before writing the codeplug. Contains 12bytes of parameters. E.g., 
`00 01 00 01 da 8c 7f f6 00 01 c2 00`, again contains 0001c200h=115200.
As not the entire codeplug file is written to the device, `00 01 da 8c` may encode the amount to be
written. 

Response only contains two bytes. E.g., `01 00`.

### 0103h Command
Only seen after codeplug write. Contains 3 parameter bytes `01 00 00`.

