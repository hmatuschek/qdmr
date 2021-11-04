# Retevis RT-73 Protocol

The protocol appears to be a simple serial interface via USB. The protocol is very high-level. That is, the majority of information on where to read/write the codeplug and its size is not transferred between the host and device. 

## Codeplug write

| From   | To     | Content                                                                                 |
| ------ | ------ | --------------------------------------------------------------------------------------- |
| Host   | Device | Init write flash, starts with `Flash Write`, 31bytes long.                              |
| Device | Host   | Several 0-terminated strings. e.g., `  Write_2M_`, `109E.D4.EARSAB.007.Apr 19 2021`, `20:44:29`, `+DRS-300UV+136-480MHZ+`. The last string is terminated and filled with `0xff` |