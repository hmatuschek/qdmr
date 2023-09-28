# GD73 Serial Protocol

## General Request Frame Format
```
     0   1   2   3   4   5   6   7   8      n-2 n-1  bytes
   +---+---+---+---+---+---+---+---+---+...+---+---+
00 |68h|FLG|CMD|SUB| CRC?  | Size  |  Payload  |10h|
   +---+---+---+---+---+---+---+---+---+...+---+---+ 
```

| Bytes  | Value | Description |
|:-------|:------|:------------|
| 0      | 68    | Start of frame byte. Fixed. |
| 1      | 0f    | Flags? |
| 2:3    | 01 04 | First command send on read. Probably *enter prog mode*. No payload. |
|        | 01 02 | Second command send on read. Probably *identify*. No payload. |
|        | 01 00 | Write request, usually 55bytes payload. |
|        | 04 01 | Read request, 2bytes payload likely address or block number in LE. |
| 4:5    |       | Likely some sort of checksum. |
| 6:7    |       | The payload size in little endian. |
| 8:(n-2)|       | The request/response payload. |
| n-1    | 10    | The end-of-frame byte. |


## General Response Frame Format
```
     0   1   2   3   4   5   6   7   8      n-2 n-1  n      n+m  bytes
   +---+---+---+---+---+---+---+---+---+...+---+---+---+...+---+
00 |68h|FLG|00h|PAD| CRC?  | Size  |  Payload  |10h| Pad Bytes |
   +---+---+---+---+---+---+---+---+---+...+---+---+---+...+---+
```

| Bytes  | Value | Description |
|:-------|:------|:------------|
| 0      | 68    | Start of frame byte. Fixed. |
| 1      | 0f    | Flags? |
| 2      | 00    | Indicates a response, probably success. |
| 3      |       | Number of additional bytes added to the end of the frame after the EOF byte. |
| 4:5    |       | Likely some sort of checksum. |
| 6:7    |       | The payload size in little endian. |
| 8:(n-2)|       | The request/response payload. |
| n-1    | 10    | The end-of-frame byte. |
| n:n+m  |       | Additional bytes observed on the *write* and *enter prog mode* responses. |

### CRC
The CRC is computed over the entire packet, including start and end bytes. For the computation, 
the packet is interpreted as a sequence of little-endian unsigned 16-bit integer. If the packet 
does not align with 16bits, it gets 0-padded to a multiple of 16bit. 

Let `v[i]` the i-th of `N` unsigned 16bit integer formed by the 0-padded packet, the CRC is then 
computed as
```
uint32_t crc = 0xffff;
for (int i=0; i<N; i++) {
  if (crc < v[i])
    crc += 0xffff;
  crc -= v[i];
}
```
To check the CRC, the resulting value of `crc` must be 0, to compute the CRC, just set the CRC field
to 0, compute the CRC over the entire packet and then set the field within the packet to the value 
of the result.


## Enter program mode
