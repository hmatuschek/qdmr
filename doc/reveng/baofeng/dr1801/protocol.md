# DR-1801A6 (BF1801) - Protocol

A new version of the popular DM-1801 by Baofeng. Is sold as DR-1801UV or DR-1801A6 and calls itself
BF1801. The communication is based on USB CDC-ACM (serial over USB). 

This device uses the AUCTUS A6 chip and thus its protocol. For a detailed description, see jhart99s 
brilliant [article series](https://jhart99.com/a6-hidden-interface/) on this chip.

## Basic structure
The first few packets exchanged at a read:
```
< a1 20 00 00 00 00 02 00 80 1e 

> aa 06 01 04 03 bb 
< aa 07 81 04 01 83 bb

> aa 07 00 2b 00 2c bb 
< aa 07 80 2b 02 ae bb 

> aa 0a 01 00 00 01 c2 00 c8 bb 
< aa 15 81 00 01 00 01 dd 90 00 00 00 68 00 02 e6 9e 09 2d ef bb 

< a1 20 00 00 00 00 02 00 00 1e
< a1 20 00 00 00 00 02 00 80 1e

> aa 06 01 01 06 bb 
< ... only packets from device to host from now on with no structure. Just plain data.
< a1 20 00 00 00 00 02 00 00 1e
```

The first few packets exchanged at a write:
```
< a1 20 00 00 00 00 02 00 80 1e 

> aa 06 01 04 03 bb
< aa 07 81 04 01 83 bb 

> aa 07 00 2b 00 2c bb 
< aa 07 80 2b 02 ae bb

> aa 12 01 02 00 01 00 01 da 8c 7f f6 00 01 c2 00 0d bb
< aa 08 81 02 01 00 8a bb 

< a1 20 00 00 00 00 02 00 00 1e 
< a1 20 00 00 00 00 02 00 80 1e

> .... Codeplug is written in many large packets with no structure, just data.

< aa 09 81 03 01 00 00 8a bb

< a1 20 00 00 00 00 02 00 84 1e
< 00 | .
< a1 20 00 00 00 00 02 00 90 1e 
< a1 20 00 00 00 00 02 00 84 1e 
< 00 | .
< a1 20 00 00 00 00 02 00 90 1e 
< a1 20 00 00 00 00 02 00 84 1e 
< 00 | .
< a1 20 00 00 00 00 02 00 90 1e 
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
`01 00 01 dd 90 00 00 00 68 00 02 e6 9e 09 2d`. 0x0001dd90 is the size of the codeplug file. 

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


## Status messages
The radio sends some status messages in between. Maybe to indicate the state of the radio. Like 
"ready" or "busy". Communication only starts/continues after 
```
< a1 20 00 00 00 00 02 00 80 1e 
```
has been received and stops whenever 
```
< a1 20 00 00 00 00 02 00 00 1e
```
has been received.

This might be used by the radio to implement some sort of flow-control. 
