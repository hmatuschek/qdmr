# AnyTone CPS codeplug file format

The fileformat used by the CPS to store the codeplug in binary format is unfortunately not directly related to the binary representation of the codeplug written to the device (in contrast to the fileformat for the GD77, RD5R and MD-UV390). This document tries to collect my knowledge about this file format.


## General layout
The geneal layout of the file consists of a header followed by the sequence of elements (usually lists) that form the codeplug. Each list starts with the number of elements in this list. At the end, a footer is added to the file. The order of the elements in the file is important. Due to the variable size of each element, the file-offsets differ from codeplug to codeplug. 

The order of elements in the file is 

  * Header (fixed size)
  * Channel list
  * Radio ID list
  * Zone list
  * List of scan lists

The format of each element is documented below.


## Header
```
   0                               8                               16
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
00 | CPS version       | Content size  | Model name                |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
10 | 0   0   0 | HW version    |                                   |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
20 |                                                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
30 |                                                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
40 |                                                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
50 |                                                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
60 |                                                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
70 |                                                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
80 |                                                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
90 |                                                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
a0 |                                                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
b0 |                                                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
c0 |                                                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
d0 |                                                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
e0 |       |
   +---+---+
```

| Field  | Description |
|---|---|
| CPS Version | Version number of the CPS as a string E.g., "V1287" |
| Content size | Content size in bytes, little-endian. |
| Model name | Model name as a string. E.g., "D878UV" |
| HW Version | Hardware version as a string. E.g., "V100" |


## Channel list
The channel list consists of a simple list of all channels concatenated together preceded by the number of channels encoded as little endian.

```
 +---+---+---+---+---+---+ ... +
 | #Chan | Channel data    ... |
 +---+---+---+---+---+---+ ... +
```

### Channel encoding
Each channel is encoded in a variable size binary blob as

```
   0                               8                               16
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
00 | ChIdx | RX frequency  |RPT| TX offset     |TYP|PWR|BWD| 0 |RXO|
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
10 |CAC|TKA| RXCTC |       | TXCTC |                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
20 |ADM|                           |CLC|   |TSL|IGS|       |WAL|   |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
   +---+---+---+---+---+---+...+---+
30 | Channel name N chars   ...  0 |
   +---+---+---+---+---+---+...+---+
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
   | 0 |RNG| ? | 0   0 |APT| 0 |FreqCor| 0 |SMC|ExR| 0   0   0   0 |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
   | 0 |DAK| 0   0 | 
   +---+---+---+---+

```

| Field  | Description |
|---|---|
| ChIdx | Specifies the channel index in little endian, 0-based. |
| RX frequency | RX frequency in 10Hz, littel endian. |
| RPT | Repeater type, 0=simplex, 1=positive, 2=negative |
| TX offset | TX offset frequency in 10Hz, little endian. Sign determined by RPT field. |
| TPY | Channel type, 1=digital, 0=analog, ... |
| PWR | Power setting 0=low, 1=mid, 2=heigh, 3=turbo. |
| BWD | Analog band width 0=12.5kHz, 1=25kHz. |
| RXO | RX only flag. |
| CAC | Call confirm flag. |
| TKA | Talkaround flag. |
| RXCTC | RX CTCSS/DSC settings. |
| TXCTC | RX CTCSS/DSC settings. |
| ADM | Admit criterion, 0=always, 1=, 2=, 3=same color code. Also busy lock 1=repeater, 2=busy.|
| COC | Color code. |
| TSL | Time slot, 0=TS1, 1=TS2 |
| IGS | Ignore time slot flag. |
| WAL | Work alone flag. |
| Channel name | Channel name of max 16 ASCII chars. Variable size (max. 16b?). Zero padded. |
| RNG | Ranging flag. |
| APT | APRS report type, 0=none, 1=analog, 2=digital. |
| FreqCor | Frequency correction in Hz, little endian. |
| SMC | SMS confirmation flag. |
| ExR | Exclude from roaming flag. |
| DAK | Data ACK flag. |


## RadioID list
As usual, the radio ID list starts with the number of defined IDs (little endian) and is followed by the actual list of IDs.
```
 +---+---+---+---+---+---+ ... +
 | #IDs  | RadioID data    ... |
 +---+---+---+---+---+---+ ... +
```

### Radio ID encoding
```
+---+---+---+---+---+---+---+ ... +---+
| Index | DMR ID    | Name          0 |
+---+---+---+---+---+---+---+ ... +---+
```

| Field  | Description |
|---|---|
| ChIdx | Specifies the radio ID index in little endian, 0-based. |
| DMR ID | The DMR ID as a 24bit little endian integer. |
| Name | Variable length radio ID name. ASCII, 0-terminated. |


## Zone List
As usual, the zone starts with the number of defined zones and is followed by the actual list of zones.
```
 +---+---+---+---+ ... +
 |#ZO| Zone data   ... |
 +---+---+---+---+ ... +
```

### Zone encoding
```
+---+---+---+---+---+---+ ... +---+---+---+---+---+---+---+ ... +---+
|ZIX|NCH| Ch00  | Ch01  | ... | Chn A | Chn B | Zone name   ...   0 |
+---+---+---+---+---+---+ ... +---+---+---+---+---+---+---+ ... +---+
```

| Field  | Description |
|---|---|
| ZIX | Zone index. |
| NCH | Number of channel in zone. |
| ChXX | List of 16bit, little endian, 0-based channel indices |
| Chn A | Zone channel A |
| Chn B | Zone channel B |
| Zone name | Name of the zone, ASCII 0-terminated. |


## Scan lists
As usual, the scan lists starts with the number of scan lists and is followed by the actual scan lists.
```
 +---+---+---+---+---+ ... +
 |#SL| Scan list data  ... |
 +---+---+---+---+---+ ... +
```

### Scan list encoding
```
+---+---+---+ ... +---+---+---+---+---+---+---+---+---+---+---+---+
|SLI| Name    ...   0 |                                           |
+---+---+---+ ... +---+---+---+---+---+---+---+---+---+---+---+---+
+---+
|NCH| 
+---+
```

| Field  | Description |
|---|---|
| SLI | Scan list index. |
| Name | Name of the scan list, max 16 x ASCII, 0-terminated. |
| NCH | Number of channels in scan list. |


