# Hytera PD785G codeplug format

## Differential analysis

It appears as though the act of writing the same codeplug to the radio does
cause diffs. The following diff is generated:

```diff
10c10
< 00000090 80 a1 03 1c e5 07 03 16 02 17 db 07 03 0a 0a 21 | ...............!
---
> 00000090 80 a1 03 1c e5 07 03 16 02 1a db 07 03 0a 0a 21 | ...............!
47246,47248c47246,47248
< 000B88D0 00 00 32 61 04 00 20 00 00 00 36 00 00 00 94 60 | ..2a.. ...6....`
< 000B88E0 a7 95 ae f2 0b e5 76 0d b6 ea 92 34 09 df 93 9c | ......v....4....
< 000B88F0 67 e9 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | g...............
---
> 000B88D0 00 00 32 61 04 00 20 00 00 00 36 00 00 00 87 79 | ..2a.. ...6....y
> 000B88E0 47 23 97 7f 40 b6 2b 18 5e b8 71 f4 ce a3 c8 6e | G#..@.+.^.q....n
> 000B88F0 48 d0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | H...............
47256,47257c47256,47257
< 000B8970 62 d0 76 59 6f 9d ad 82 6e 24 bd 8c 5d ec c2 c1 | b.vYo...n$..]...
< 000B8980 2b 95 55 dd 9c dd 3a d1 1a 63 00 fa 79 cd 1a e4 | +.U...:..c..y...
---
> 000B8970 62 d0 76 59 6f 9d da 27 e1 84 ae 1c e0 b2 92 e6 | b.vYo..'........
> 000B8980 8b 9a 07 49 98 8d be c4 d8 69 00 fa 79 cd 1a e4 | ...I.....i..y...
```

We can therefore disregard any changes seen at these addresses when changing CP
values.  Any diffs shown in this document will filter out these changes.

### Programming time stamp 
Haveing a look at the transferred data, there is a dedicated write of 6 bytes to address 
0x00000094. Here this would correspond to 
```
e5 07 03 16 02 17
```
being replaced by 
```
e5 07 03 16 02 1a
```
 - 07e3 03 16 02 17-> 2021-03-16, 02:23
 - 07e3 03 16 02 1a-> 2021-03-16, 02:26

### SHA1 HMACs?!?
The other two changes are certainly worrying: These are both 160bit blocks and they change completely without changing the code-plug at all (except for the suspected programming time stamp). This would imply a cryptographic hash function over parts or the compete code-plug. A likely candidate would be SHA1. 

The latter, however, does not make sense as a kind of checksum. This can be done cheaper using CRC32, in particular as many MCUs have dedicated hardware to compute such checksums. SHA1 would make sense if it is used as a HMAC. If this turns out to be true, then we will never be able to programm these Hytera devices.

## High level structure (Sections)

The codeplug appears to be split into numerous sections that are contagious in
memory.  Each section contains three parts:

 - Header
 - Data
 - Mapping table
 
Note you can use the `printSections` cps command to print out the sections. For
example:

```
matthew@prax $ python cps.py -i ./my-cp.bin printSections
Section: address=0x392 type=02 cap=1 elem_in_use=1 size=180  unk1=0x00 unk2=0x6132 unk3=0x04
Section: address=0x462 type=03 cap=1 elem_in_use=1 size=48  unk1=0x00 unk2=0x6132 unk3=0x04
Section: address=0x4AE type=77 cap=1 elem_in_use=1 size=68  unk1=0x00 unk2=0x6132 unk3=0x04
Section: address=0x50E type=76 cap=1 elem_in_use=1 size=32  unk1=0x00 unk2=0x6132 unk3=0x04
Section: address=0x54A type=04 cap=1 elem_in_use=1 size=4  unk1=0x00 unk2=0x6132 unk3=0x04
Section: address=0x56A type=05 cap=1 elem_in_use=1 size=8  unk1=0x00 unk2=0x6132 unk3=0x04
Section: address=0x58E type=06 cap=1 elem_in_use=1 size=16  unk1=0x00 unk2=0x6132 unk3=0x04
Section: address=0x5BA type=07 cap=1 elem_in_use=1 size=8  unk1=0x00 unk2=0x6132 unk3=0x04
Section: address=0x5DE type=08 cap=1 elem_in_use=1 size=100  unk1=0x00 unk2=0x6132 unk3=0x04

...
```

### Section Header

The format of a section header is:

```C++
strcut section_header {
  uint16_t kind;
  uint16_t capacity;
  uint16_t num_elements : 12,
           unk1 : 4;
  
  uint16_t unk2; // seems to always be 0x0020
  uint16_t unk3; // seems to always be 0x0000
  uint16_t unk4;
  uint16_t unk5;
  uint16_t data_size; // in bytes
  uint16_t data_size_2; // seems to always be `data_size + 0x16`.
}
```

Where:
  - `kind`: This value seems to indicate which kind of data the section contains.
  - `capacity`: This is the number of *elements* that the section can store. The
    size of the elements depends upon the `kind` of data stored in this section.
  - `num_elements`: The number of elements contained within the data.
  - `unk1`: This value seems to be `8` when the data doesn't contain elements
    that are packed together.
  - `unk2`: Observed to be `0x0020`.
  - `unk3`: Observed to be `0x0000`.
  - `unk4`: Seems to have different values depending upon the codeplug used.
  - `unk5`: Seems to have different values depending upon the codeplug used.
  - `data_size`: The size of the data section, in bytes, not including the hader.
  - `data_size_2`: Seems to always have the value of `data_size + 0x16`, for
    some strange reason.
    
### Data

The section data follows after the header and is `data_size` bytes long. It
normally contains `num_elements` elements of data and the size of each element
depends upon the `kind` of data that this section stores.

### mappings

At the end of each section there appears to be a mapping table. This table
allows elements of data in this section to be referenced from other sections in
the codeplug.  Each mapping has the following structure:

```C++
struct section_mapping
{
  uint16_t idx;
  uint32_t offset;
}

```

The `offset` field appears superfluous, as it can be calcuated as `idx *
sizeof(element kind)`. The *index* in the mapping list is important as this
defines the mapping. As an example. Suppose we have the following list of
mappings (only `idx` fields shown);

```
mappings = [7,26,84,3,2]
```
*Note* It has been observed that reference indices map into the mappings
table at a position of `-1`.

And we have a refence in some other structure that has a value of `2`. In that
case, the index of the element in the data that we should link to is at offset
`26`, since:

```
section_data[mappings[2 - 1]]
```

## DMR ID

Changing the DMRID of the radio (from 1 to 12345678) results in the following
changes:

```diff
26027c26027
< 00065AA0 04 00 4c 00 00 00 62 00 00 00 01 00 00 00 10 06 | ..L...b.........
---
> 00065AA0 04 00 4c 00 00 00 62 00 00 00 4e 61 bc 00 10 06 | ..L...b...Na....
26029c26029
< 00065AC0 00 00 00 00 20 00 fc 01 4e 44 14 00 5f 00 00 00 | .... ...ND.._...
---
> 00065AC0 00 00 00 00 20 00 fc 01 4e 44 14 00 a1 00 00 00 | .... ...ND......
54784c54784
< 000D5FF0 ff 00 00 00 00 00 0a 00 00 01 0a e1 00 00 ff 00 | ................
---
> 000D5FF0 ff 00 00 00 00 00 0a bc 61 4e 0a e1 00 00 ff 00 | ........aN......

``` 

- The first diff indicates that the DMRID is stored at address 0x65aaa as three
  bytes LE.
  
- The second diff indicates a byte has changed at address 0x65acc, perhaps some
  kind of checksum?
  
- The third diff shows that the DMRID is stored at address 0xd5ff7 as three
  bytes BE.

## Channels
### Frequency Format (`freq_t`)

Frequencies appear to be stored as 32-bit unsigned fixed-point LE values. For
example the frequency 450.71250MHz is encoded as `b4 53 dd 1a`, which when
interpreted as a 32-bit unsigned LE integer yields `450712500`.

## Digitial
### Layout

Note all fields that are greater than one byte have LE byte ordering.

```c
typedef uint32_t freq_t // 32-bit fixed point frequency

struct digital_chan_t
{
  // Using enums to translate meaning to values
  enum TimeSlot {
    TIMESLOT_1 = 0,
    TIMESLOT_2 = 1
  };

  enum Power {
    POWER_LOW  = 1,
    POWER_HIGH = 0
  };

  enum TxAllowed {
    TX_ALLOWED = 0,
    RX_ONLY = 1
  };

  enum TxAdmit : uint8_t {
    ALWAYS = 0,
    CHANNEL_FREE = 1,
    COLOUR_CODE_FREE = 2
  };

  enum TxTimeout : uint8_t {
    INFINITE = 0,
    SECS_60 = 0x0c,
  };

  // byte 0
  char name[32];
  // byte 32
  uint8_t unk32;
  // byte 33
  // Using bitfields for single flags from LSB to MSB
  uint8_t rx_only    : 1,    /// 1=rx ony, 0=tx allowed
          unk33      : 2,
          power      : 1,    /// 1=High, 0=Low
          ukn33_5    : 4;    /// <- Althoug not needed, it helps me to check whether I've got them all.
  // byte 34
  uint16_t unk34;
  // byte 36
  freq_t rx_freq;
  // byte 40
  freq_t tx_freq;
  // byte 44
  TxAdmit tx_admit; 
  // byte 45
  TxTimeout tx_timeout;
  // byte 46
  uint8_t tx_timeout_prealert_secs;
  // byte 47
  uint8_t tx_timeout_rekey_secs;
  // byte 48
  uint8_t tx_timeout_reset_secs;
  // byte 49
  uint8_t colour_code : 4,
          unk34_1 : 1,
          prority_interrupt_encode : 1,
          prority_interrupt_decode : 1,
          unk34_2 : 1;
  // byte 50
  uint16_t tx_contact_idx;
  // byte 52
  uint16_t rx_group_list_idx;
  // byte 54
  uint16_t emergency_system_idx;
  // byte 56
  uint8_t scan_chan_idx;
  // byte 57
  uint8_t unk57;
  //byte 58
  uint8_t timeslot    : 2,    /// 2=pseudo trunk (both timeslots) 1=TS2, 0=TS1
          ukn56_1     : 2,    /// unknown set to ???
          has_scan_ch : 1,    /// 1=scan_chan_idx is used, 0=ignore scan_chan_idx
          vox         : 1,    /// 1=Enabled, 0=Disabled.
          unk58_2     : 1,
          option_board: 1;    /// 1=enable, 0=disable
  // byte 59
  uint8_t unk59[3];
  // byte 62
  
  // 0xffff = None
  // 0x0000 = Selected
  // otherwise idx = loc_revert_channel - 1
  uint16_t loc_revert_channel;
  // byte 64
  uint8_t unk64[2]
  // byte 66
  uint16_t phone_system_idx;
  // byte 68
  
  // 0x00 = None
  // 0x01 = slot 1
  // 0x02 = slot 2
  uint8_t designated_pseudo_trunk_tx;
  // byte 69
  uint8_t unk68[3];

  //byte 72
  // 0xffff = None
  // 0x0000 = Selected
  // otherwise idx = rrs_revert_channel - 1
  uint16_t rrs_revert_channel;
};
```

### TX and RX frequency

For the same digitial channel an RX frequency was seen to change at address
`0x3ab4e`. The TX frequency was seen to change at address `0x3ab52`.

### Mappings
Digitial channels can be 'pointed to' by other portions of CP memory, for example zones.  Referencing channels is done by a `uint16_t` value.  This value *does not* refer to the index of a channel in the channel list.  Instead the CP memory contains a set of mappings that defines a seperate value for each channel.  These mappings are located at the end of the digitial channel list, at address `0x0004fad6` and has the format:

```C
struct ChannelMapping
{
  uint16_t idx;
  uint32_t offset;
};
```

The `idx` and `offset` of the `ChannelMapping` structure index into the channel
list array, however it is the position of the `ChannelMapping` object in the mappings array that defines the mapping value.  For example, suppose we had the following memory  at address `0x0004fad6`:

```
5500 e41b 0000 5400 901b 0000 5300 3c1b 0000
```

The following mappings are then defined:

| Mappning Index | Channel Array Index |
| 0              | `0x55`              |
| 1              | `0x54`              |
| 2              | `0x53`              |

## Digitial Contacts

Contacts start at address `0x65b70` and runs to `0x71b70` thus allowing for a
maximum of 1024 contacts. Each record has the format:

```c
typedef uint16_t unicode_char_t;

struct gigital_contact_t
{
  enum call_type_t : uint8_t {
    PRIVATE_CALL   = 0,
    GROUP_CALL     = 1,
    IGNORE_CONTACT = 0x11
  };
  
  uint16_t index;           /// Endianess?
  unicode_char_t name[16];
  call_type_t call_type;    
  uint8_t is_referenced;
  uint16_t __pad_1;
  uint32_t id;              /// Endianess?
  uint32_t unk1;
  uint16_t link;            /// Endianess?
};

```

The `is_referenced` field has a value of `1` when any digital channel references
this contact and `0` otherwise.

### Contact traversal

The number of contacts in the list is stored in address `0x65b5c` as a LE
uint16_t. This number denotes the number of elements, so to get the size of the
table to download it needs to be multiplied by `0x30`.

A 'dummy' node is created that links back to index 0. Notice on this node that
`call_type` is `0x11`. My assumption here is that the value `0x11` on call type
indicates this is a dummy node and should be ignored.

The use of the `link` and `idx` fields is currently unknown. It was assumed that
the `index` field would be used to link into the table by digital channels.
However, it appears as though this is simply done based upon an offset into the
table. This leaves the function of `index` and `link` currently a mystery.

## Zones

Zones appear to be split into two parts of memory. First, a list of zone names
is present at address `0x39764`. For each zone name sgment, there is a
corresponding zone list segment at `0x125c + (zone_name_idx * 0xe0e)`.

### Zone Names
The number of zones is found at address `0x39764 - 0x12`. Each zone name has the
following format:

```C
struct ZoneName
{
  char name[32];
  uint16_t num_zone_entries;
  uint8_t zone_list_data[6];
};
```

Where:
  - `name` is the name of the zone in unicode format.
  - `num_zone_entries` is the number of channels listed in the zone.
  - `zone_list_data` is a copy of the first six bytes of the zone_list data for
    this particular entry.

### Zone Lists
Each zone name entry has a Zone list entry. Each zone list entry defines the
channels in the zone and contains a contagious list of `ZonePointer` entries:

```C
struct ZonePointer
{
  uint16_t channel_ref;
  uint16_t is_analog;
};
```

Where
  - `channel_ref` is a number that references a channel through the channel
    mappings.
  - `is_analog`: `0` when a digitial channel, `1` when analog. Other values have
    not been observed.
