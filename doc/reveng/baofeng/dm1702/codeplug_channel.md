# Channel encoding
The channels are stored across several blocks. At the time of this writing, it is known that the channel names are stored separately
from the channel properties.

## Channel names
The channel names are stored in the block at `0x007000` (`0x004000` in CPS file) as a continuous 
list of 10-byte ASCII names separated by a 0-byte. If the name is shorter than 10 bytes, the name 
gets 0-padded to 10bytes. 

## Channel properties
The channel properties are stored in the block `0x077000` (`0x003000` in CPS file). The first 
16 bytes are some sort of a header. The very first byte at `0x077000` holds the channel counter.
```
      7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0     
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 00 | Channel count                 | Unused set to 0                                                                            ...
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 0c  ...                                                                                                                            |
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
``` 

Each channel occupies 0x30 bytes and is directly appended to the header above within the block `0x077000`. That is, the offset for the 
first channel properties is `0x077010`. The channel properties are then encoded as 
```
      7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0     
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 00 | RX frequency in 10Hz as 8-digit BCD, little endian.                                                                           |
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 04 | TX frequency in 10Hz as 8-digit BCD, little endian.                                                                           |
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 08 | 0 |CHT|RXO|SQT| 0 |LWE|PWR| 0 |ASE|CBW| Scan list index + 1   | 0   0   0 | DAdm  | 0 | AAdm  |EAT| 0   0   0 | EMSys idx+1   |
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 0c | 0   0   0   0   0   0   0   0 | 0   0 |PCC|DCC| 0   0   0   0 | 0   0 |DCM|TS2| Color Code    |ENE| 0   0   0 | Key index + 1 |
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 10 | GPS System index +1           | Group list index +1           | Unknown, set to 0x01          | RX Tone                    ...
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 14  ...                            | TX Tone                                                       | 0   0 |VOX| 0   0   0   0   0 | 
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 18 |DPI| 0   0   0   0   0   0   0 | Unknown, set to 0x0000                                        | 0 |TAS| 0   0   0   0   0   0 |
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 1c | Unknown, set to 0x00000000                                                                                                    |
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 20 | Auto reset time in sec.       | Unknown, set to 0x0a          | Unknown, set to 0x51          | Unknown, set to 0x00          |
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 24 | Unknown, filled with 0x00                                                                                                  ...
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 2c  ...                                                                                                                            |
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
```

| Offset | Type | Field | Description |
| ------ | ---- | ----- | ----------- |
| 00:00  | BCD  | RX Frequency | Specifies the RX frequency in multiples of 10Hz as an 8-digit BCD number, stored in little endian. |
| 04:00  | BCD  | TX Frequency | Specifies the RX frequency in multiples of 10Hz as an 8-digit BCD number, stored in little endian. |
| 08:00  | bit  | CBW | Channel bandwidth. 0=narrow (12.5kHz), 1=wide (25kHz, analog only). |
| 08:01  | bit  | PWR | Power setting, 0=low, 1=high. |
| 08:02  | bit  | LWE | Lone worker enable. |
| 08:04  | bit  | SQT | Squelch type 0=General, 1=Stringent. |
| 08:05  | bit  | RXO | RX only enable.
| 08:06  | bit  | CHT | Specifies the channel type, 0=digital, 1=analog. |
| 09:07  | bit  | ASE | Auto scan enable. |
| 09:00  | uint | Scan list index | Scan list index+1, 6bit uint, 0=none. |
| 0a:03  | uint | DAdm | DMR Admit criterion, 2bit uint, 0=Always, 1=Free, 2=CC. |
| 0a:00  | uint | AAdm | FM Admit criterion, 2bit uint, 0=Always, 1=Free, 2=Tone correct, 3=Tone incorrect. |
| 0b:00  | uint | EMSys idx | Emergency system index+1, 4bit uint, 0=none. | 
| 0b:07  | bit  | EAT | Emergency alarm tone enable. |
| 0d:04  | bit  | DCC | Data call confirm enable. |
| 0d:05  | bit  | PCC | Private call confirm enable. |
| 0e:00  | uint | Color code | Specifies the colo code, 4bit uint, [0,15] |
| 0e:04  | bit  | TS2 | Time-slot, 0=TS1, 1=TS2. |
| 0e:05  | bit  | DCM | Dual-capacity direct mode enable. |
| 0f:00  | uint | Key index | Specifies the encryption key index + 1, 4bit uint, 0=none. |
| 0f:07  | bit  | ENE | Encryption enable. |
| 10:00  | uint | GPS index | Specifies the GPS system index +1, 8bit uint, 0=none. |
| 11:00  | uint | Group list idx | Specifies the group-list index +1, 8bit uint, 0=none. |
| 13:00  | BCD  | RX Tone | Specifies the RX tone CTCSS and DCS. For CTCSS tones are stored as 0.1Hz values as well as the DCS codes in 3-digit BCD little endian (16bit). The leading 4th digit specifies the type 0x0=CTCSS, 0x8=DCS non-inverted, 0xc=DCS inverted.
| 15:00  | BCD  | RX Tone | Specifies the RX tone CTCSS and DCS. For CTCSS tones are stored as 0.1Hz values as well as the DCS codes in 3-digit BCD little endian (16bit). The leading 4th digit specifies the type 0x0=CTCSS, 0x8=DCS non-inverted, 0xc=DCS inverted.
| 17:04  | bit  | VOX | VOX enable. |
| 18:07  | bit  | DPI | Display PTT ID enable. |
| 1b:06  | bit  | TAS | Talk around status enabled. |
| 20:00  | uint | Auto reset time | Specifies the auto-reset time (TOT?) in seconds. [1,255] |

## DMR default contacts
The default TX contact for each channel is stored separately. Of cause they are, it makes 
absolutely sense to store everything at one place with plenty of spare room left, except for the 
channel names and the transmit contacts. That take 12bytes to store, while the channel 
settings-block has about 18 unused bytes. Anyway, we have to deal with this genius artwork of 
embedded system design. The default transmit contacts for the channels are found at `0x0ca000` 
(`0x01e000` within the CPS file). The format is a simple list of 16bit indices in little endian.
That is,
```
      7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0     
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 00 | Channel 0, TX contact index + 1, 16bit uint little-endian.    | Channel 1, TX contact index + 1, 16bit uint little-endian     |
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     ...                                                                                                                         ...
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
7fc | Channel 1022, TX contact index + 1                            | Channel 1023, TX contact index + 1                            |
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
```
Unused contact indices (i.e., of unset channels) are set to 0xffff. A value of 0x0000 indicates 
that the channel has no default contact index.