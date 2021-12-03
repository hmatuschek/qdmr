# Codeplug
The codeplug address-space within the radio spans `0x002000 -- 0x0FB000` and is organized in blocks
of `0x1000` bytes. Before a codeplug is read or written the last byte of each block gets read from 
the device. If the last byte is either `0x00` or `0xff`, the block is considered empty and will not
be read. The value of the last byte differs from block to block (the ones being read). The mechanism
behind this is not known yet. Fortunately, these *marker bytes* are also set in the manufacturer CPS 
file.

## Memory & File Layout
Fortunately, the CPS codeplug file does contain the raw binary codeplug that is written to the 
device. Unfortunately, it is not a simple one-to-one dump of the radios memory. None the less, it
allows for some decent reverse engineering of the codeplug elements and will also allow for some
simple means to import the manufacturer codeplug file into qdmr.


| Mem From | Mem To | File From | File To | Content |
| -------- | ------ | --------- | ------- | --------|
| 005000   | 006000 | 00A000    | 00B000  | Preset messages. |
| 007000   | 008000 | 004000    | 005000  | Channel Names. |
| 009000   | 00D000 | 
| 00A000   | 00B000 | 008000    | 009000  | Other Lists, EmSys, Priv. |
| 00D000   | 00E000 | 00B000    | 00C000  | Lists |
| 00E000   | 00F000 | 00C000    | 00D000  | Lists |
| 00F000   | 010000 | 
| 010000   | 011000 | 00E000    | 00E000  | 
| 011000   | 012000 | 00F000    | 010000  | 
| 012000   | 01E000 | 010000    | 01C000  | 
| 01E000   | 020000 | 01C000    | 01E000  |
| 022000   | 026000 | 020000    | 024000  | Call172- |
| 026000   | 027000 | 025000    | 026000  | 
| 02C000   | 03F000 | 
| 040000   | 065000 | 
| 077000   | 078000 | 
| 07E000   | 07F000 | 
| 090000   | 091000 | 006000    | 007000  | Zones |
| 095000   | 096000 | 
| 09C000   | 09E000 | 01F000    | 020000  | Call1-171 |
| 0A1000   | 0A2000 | 005000    | 006000  | Unknown, Timestamp? |
| 0CA000   | 0CB000 | 01E000    | 01F000  | 

