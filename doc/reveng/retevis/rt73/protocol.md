# Retevis RT-73 (Kydera) Protocol

The protocol appears to be a simple serial interface via USB. The protocol is very high-level. That is, the majority of information on where to read/write the codeplug and its size is not transferred between the host and device. 

In fact this device is also known as the *Radioddity DB25-D* and *Kydera CDR-300UV*. As this device identifies itself as a `DRS-300UV+136-480MHZ` after entering the programming mode, I suspect that Retevis and Radioddity just re-sell a re-branded Kydera device. To this end, the protocol and base class for these devices will be called `kydera` in libdrmconf.

## Codeplug write

| From   | To     | Content                                                                                 |
| ------ | ------ | --------------------------------------------------------------------------------------- |
| Host   | Device | Init write flash, starts with `Flash Write`, 31bytes long.                              |
| Device | Host   | Several 0-terminated strings. E.g., `  Write_2M_`, `109E.D4.EARSAB.007.Apr 19 2021`, `20:44:29`, `+DRS-300UV+136-480MHZ+`. The last string is terminated and padded with `0xff` |
| Host   | Device | 8 plain binary blobs of the codeplug. Each write packet contains 0x100 bytes. |
| Device | Host   | After 0x800 bytes written, the device ACKs with `Write` |
| Host   | Device | 8 more plain binary blobs of the codeplug. |
| ...    | ...    | This continues on ... |
| Device | Host   | Once the entire codeplug is written (0x1c0 blocks, 0x1c000 bytes in total), the device sends a checksum `ChecksumW` + 32bit uint and a status report to the host. |

### Flash Write Command
This command is send first by the host to the device to initialize the flash write.
```
   0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f  
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
00 | 'Flash Write' ASCII                                  | 00 | 3c | 00 | 00 | 00 |
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
10 | 00 | 00 | 00 | 38 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 |
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+     
```
Except for the beginning ASCII command text, the remaining bytes are unknown. 

### Device info 
The following is send as a response by the device to a `Flash Write` command
```
   +----+----+
00 | 20 | 20 |
   +----+----+----+----+----+----+----+----+----+----+
02 | 'Write_2M_'                                | 00 | 
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
0c | '109E.D4.EARSAB.0'                                                         
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
1c  '07.Apr 19 2021'                                                     | 00   00 
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
cc   00   00   00   00   00 |
   +----+----+----+----+----+----+----+----+----+----+  
31 | '20:44:29'                            | 00   00 |
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
3b | '+DRS-300UV+136-4'                                  
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
4b   '80MHZ+'                    | ff | ff | ff | ff | ff | ff | ff | ff | ff | ff |
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
5b | ff | ff | 
   +----+----+
```


## Codeplug read

| From   | To     | Content                                                                                 |
| ------ | ------ | --------------------------------------------------------------------------------------- |
| Host   | Device | Init read flash, starts with `Flash Read`, 31 bytes long. |
| Device | Host   | Several 0-terminated string and some binary data. E.g., `  Read_2M_`, `109E.D4.EARSAB.007.Apr 19 2021`, `20:44:29`, `+DRS-300UV+136-480MHZ+`, last string `0xff` terminated and padded. |
| Host   | Device | `Read` command. |
| Device | Host   | Sends 0x800 bytes in several packets. |
| Host   | Device | `Read` command. |
| ...    | ...    | This continues on ... |
| Device | Host   | After 0x1c000 bytes have been read, the device sends the checksum `ChecksumR` followed by a 32bit uint in binary. |

### Flash Read Command 
This command is send first by the host to the device to initialize the flash read.
```
   0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f  
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
00 | 'Flash Read ' ASCII                                  | 00 | 3c | 00 | 00 | 00 |
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
10 | 00 | 00 | 00 | 37 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 |
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+     
```
Except for the beginning ASCII command text, the remaining bytes are unknown. 

### Device info
```
   +----+----+
00 | 20 | 20 |
   +----+----+----+----+----+----+----+----+----+----+
02 | 'Read_2M_'                            | 00 | 00 |
   +----+----+----+----+----+----+----+----+----+----+
0c | 01   00 | 01   00 | 08   00 | 37   00 | 01   00 |
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
16 | '109E.D4.EARSAB.0'                                                         
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
26  '07.Apr 19 2021'                                                     | 00   00 
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
36   00   00   00   00   00 |
   +----+----+----+----+----+----+----+----+----+----+  
3b | '20:44:29'                            | 00   00 |
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
45 | '+DRS-300UV+136-4'                                  
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
55   '80MHZ+'                    | ff | ff | ff | ff | ff | ff | ff | ff | ff | ff |
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
65 | ff | ff | 
   +----+----+
```