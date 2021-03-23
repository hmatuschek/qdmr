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

```c
typedef uint32_t freq_t // 32-bit fixed point frequency

typedef struct
{
    ...
    freq_t rx_freq;
    freq_t tx_freq;
    ...
} digital_chan_t;

```

### TX and RX frequency

For the same digitial channel an RX frequency was seen to change at address
`0x3ab4e`. The TX frequency was seen to change at address `0x3ab52`.

## Digitial Contacts

Contacts start at address `0x65b70` and runs to `0x71b70` thus allowing for a
maximum of 1024 contacts. Each record has the format:

```c
typedef uint16_t unicode_char_t;

// 0 - private call
// 1 - group call
// 0x11 - Ignore contact
typedef uint8_t call_type_t;

typedef struct
{
    uint16_t index;
    unicode_char_t name[16];
    call_type_t call_type;
    uint8_t is_referenced;
    uint16_t __pad_1;
    uint32_t id;
    uint32_t unk1;
    uint16_t link;
}

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
