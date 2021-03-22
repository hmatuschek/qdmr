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
    uint8_t unk0;
    uint16_t __pad_1;
    uint32_t id;
    uint32_t unk1;
    uint16_t link;
}

```

### Contact traversal

The number of contacts in the list is stored in address `0x65b5c` as a LE
uint16_t. This number denotes the number of elements, so to get the size of the
table to download it needs to be multiplied by `0x30`.

Not all slots in the table contain valid contacts; some may be empty or some may
contain old contact data. In order to filter out these contacts and find only
valid contacts, we need to use the `link` field to establish a chain of valid
contacts. The `link` field establishes a linked-list, indexed by the `index`
field. The first element in the list can be found by reading a LE uint16_t value
from `0x65b6e`; this value is the first `index` in the table. The final contact
is denoted by visiting a node that has already been visited, i.e. the end of the
list forms a loop between two contacts.

As an example, lets take the following contact data.

```
0x00065b50  0000 0000 0000 0000 2a00 0004 0300 2000  ........*..... .
0x00065b60  0000 3261 0400 00c0 0000 16c0 0000 0300  ..2a............
0x00065b70  0200 4300 6100 6c00 6c00 3100 0000 0000  ..C.a.l.l.1.....
0x00065b80  0000 0000 0000 0000 0000 0000 0000 0000  ................
0x00065b90  0000 1101 0000 0100 0000 0000 0000 0000  ................
0x00065ba0  0300 6e00 6500 7700 2000 3100 0000 0000  ..n.e.w. .1.....
0x00065bb0  0000 0000 0000 0000 0000 0000 0000 0000  ................
0x00065bc0  0000 0100 0000 0200 0000 0000 0000 0000  ................
0x00065bd0  0000 6e00 6500 7700 2000 3200 0000 0000  ..n.e.w. .2.....
0x00065be0  0000 0000 0000 0000 0000 0000 0000 0000  ................
0x00065bf0  0000 0100 0000 0300 0000 0000 0000 0000  ................
```

The value at address `0x65b5c` indicates we have a table of three elements. We
can cast this data using the structure above to obtain three channel 'slots':

```
slot[0] =
       idx : 0x00065b70 = 0x0002
      name : 0x00065b72 = "C"
 call_type : 0x00065b92 = 0x11
      unk0 : 0x00065b93 = 0x01
     __pad : 0x00065b94 = 0x0000
        id : 0x00065b96 = 1
      unk1 : 0x00065b9a = 0
      link : 0x00065b9e = 0x0000
slot[1] =
       idx : 0x00065ba0 = 0x0003
      name : 0x00065ba2 = "n"
 call_type : 0x00065bc2 = 0x01
      unk0 : 0x00065bc3 = 0x00
     __pad : 0x00065bc4 = 0x0000
        id : 0x00065bc6 = 2
      unk1 : 0x00065bca = 0
      link : 0x00065bce = 0x0000
slot[2] =
       idx : 0x00065bd0 = 0x0000
      name : 0x00065bd2 = "n"
 call_type : 0x00065bf2 = 0x01
      unk0 : 0x00065bf3 = 0x00
     __pad : 0x00065bf4 = 0x0000
        id : 0x00065bf6 = 3
      unk1 : 0x00065bfa = 0
      link : 0x00065bfe = 0x0000
```

The value at `0x65b6e` indicates that we should start the traversal at the
element with `index` 3. Thus we traverse: `3 -> 0 -> 0` and since index `0` is
one that we've previously visited we finish with the set of contacts as `3` and
`0`.

A special case should be mentioned when there is only a single channel in the
code plug. I would have assumed that the link and index on the slot in question
would have both been 0 so the channel refers to itself. However, the CPS appears
to do something different:

```
0x00065b50  0000 0000 0000 0000 2a00 0004 0200 2000  ........*..... .
0x00065b60  0000 3261 0400 00c0 0000 16c0 0000 0000  ..2a............
0x00065b70  0000 4300 6100 6c00 6c00 2000 3100 0000  ..C.a.l.l. .1...
0x00065b80  0000 0000 0000 0000 0000 0000 0000 0000  ................
0x00065b90  0000 0101 0000 0100 0000 0000 0000 0100  ................
0x00065ba0  0100 4300 6100 6c00 6c00 3100 0000 0000  ..C.a.l.l.1.....
0x00065bb0  0000 0000 0000 0000 0000 0000 0000 0000  ................
0x00065bc0  0000 1101 0000 0100 0000 0000 0000 0000  ................

slot[0] =
       idx : 0x00065b70 = 0x0000
      name : 0x00065b72 = "C"
 call_type : 0x00065b92 = 0x01
      unk0 : 0x00065b93 = 0x01
     __pad : 0x00065b94 = 0x0000
        id : 0x00065b96 = 1
      unk1 : 0x00065b9a = 0
      link : 0x00065b9e = 0x0001

slot[1] =
       idx : 0x00065ba0 = 0x0001
      name : 0x00065ba2 = "C"
 call_type : 0x00065bc2 = 0x11
      unk0 : 0x00065bc3 = 0x01
     __pad : 0x00065bc4 = 0x0000
        id : 0x00065bc6 = 1
      unk1 : 0x00065bca = 0
      link : 0x00065bce = 0x0000
```

A 'dummy' node is created that links back to index 0. Notice on this node that
`call_type` is `0x11`. My assumption here is that the value `0x11` on call type
indicates this is a dummy node and should be ignored.
