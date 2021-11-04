# Retevis RT-73 Protocol

The protocol appears to be a simple serial interface via USB. The protocol is very high-level. That is, the majority of information on where to read/write the codeplug and its size is not transferred between the host and device. 

## Codeplug write

| From   | To     | Content                                                                                 |
| ------ | ------ | --------------------------------------------------------------------------------------- |
| Host   | Device | Init write flash, starts with `Flash Write`, 31bytes long.                              |
| Device | Host   | Several 0-terminated strings. e.g., `  Write_2M_`, `109E.D4.EARSAB.007.Apr 19 2021`, `20:44:29`, `+DRS-300UV+136-480MHZ+`. The last string is terminated and filled with `0xff` |
| Host   | Device | 8 plain binary blobs of the codeplug. Each write packet contains 0x100 bytes. |
| Device | Host   | `Write` |
| Host   | Device | 8 more plain binary blobs of the codeplug... |
| Device | Host   | Once the entire codeplug is written (0x1c0 blocks, 0x1c000 bytes in total), the device sends a checksum and a status report to the host. |

