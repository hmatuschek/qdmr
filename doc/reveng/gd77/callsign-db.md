# GD77 call-sign DB memory representation
The encoding appears to be super simple:

## Header
``` 
0                               8                               
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+ ... +
|'I'|'D'|'-'|'V'|'0'|'0'|'1'| 0 | N entries     | Entries     ... |
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+ ... +
```
The first 8 bytes (at address 0x0000) contains the 0-terminated string `ID-V001`, 
followed by the number of DB entries encoded as a 32bit little endian integer. After 
this header all entries are just appended. These entries must be sorted by the DMR ID
in ascending order. 

There is no index table written to the device. 


## DB Entry
Each entry of the table is encoded into 12bytes as
``` 
0                               8                               
+---+---+---+---+---+---+---+---+---+---+---+
| DMR ID        | Name, ASCII               |
+---+---+---+---+---+---+---+---+---+---+---+
``` 
where the DMR ID is encoded as 8 BCD numbers in litte-endian and the name is max 8 ASCII chars, 0-terminated and padded.
