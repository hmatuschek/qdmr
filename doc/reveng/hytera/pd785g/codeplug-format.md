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
