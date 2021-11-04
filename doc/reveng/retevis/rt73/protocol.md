# Retevis RT-73 Protocol

The protocol appears to be a simple serial interface via USB. The protocol is very high-level. That is, the majority of information on where to read/write the codeplug and its size is not transferred between the host and device. 


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

